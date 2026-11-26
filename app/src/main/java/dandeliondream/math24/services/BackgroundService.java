package dandeliondream.math24.services;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import dandeliondream.math24.R;
import dandeliondream.math24.threads.AECMainThread;

public class BackgroundService extends Service {
    public static final int NOTIFY_ID = 0x114514;

    public static final int SHOW_ANSWER = 0;
    public static final int SHOW_PROGRESS = 1;
    public static final int SHOW_QUESTION = 2;

    public static final int ABORTED = 3;
    public static final String NOTIFY_NAME = "计算器后台服务";

    private static final Pattern P = Pattern.compile("^\\S*\\s\\S*");
    public final AECBinder binder = new AECBinder();
    public Handler displayHandler;
    public DisplayCallBack displayCallBack = null;
    private NotificationManager mNotificationManager;
    private Notification.Builder mBuilder;
    private AECMainThread aecMainThread;
    private boolean isRunningAEC = false;

    private native void setRunningState(boolean state);

    @RequiresApi(api = Build.VERSION_CODES.UPSIDE_DOWN_CAKE)
    @SuppressLint("ForegroundServiceType")
    @Override
    public void onCreate() {
        super.onCreate();
        startForeground(NOTIFY_ID, this.buildNotification(), ServiceInfo.FOREGROUND_SERVICE_TYPE_SPECIAL_USE);
        displayHandler = new Handler(Looper.getMainLooper()) {
            @Override
            public void handleMessage(@NonNull Message msg) {
                switch (msg.what) {
                    case SHOW_ANSWER:
                        if (msg.obj instanceof String) {
                            String string = (String) msg.obj;
                            if (displayCallBack != null) {
                                displayCallBack.onAnswerUpdate(string);
                            }
                        }
                        setRunningState(false);
                        aecMainThread.quit();
                        isRunningAEC = false;
                        break;
                    case SHOW_PROGRESS:
                        if (msg.obj instanceof AECProgress) {
                            AECProgress aecProgress = (AECProgress) msg.obj;
                            if (displayCallBack != null) {
                                displayCallBack.onProgressUpdate(aecProgress);
                            }
                            if (mBuilder != null && mNotificationManager != null) {
                                mBuilder.setContentTitle(aecProgress.toStringShort());
                                mNotificationManager.notify(NOTIFY_ID, mBuilder.build());
                            }
                        }
                        break;
                    case SHOW_QUESTION:
                        if (msg.obj instanceof AECQuestion) {
                            AECQuestion aecQuetion = (AECQuestion) msg.obj;
                            if (displayCallBack != null) {
                                displayCallBack.onQuestionUpdate(aecQuetion);
                            }
                        }
                        setRunningState(false);
                        aecMainThread.quit();
                        isRunningAEC = false;
                        break;
                    case ABORTED:
                        if(displayCallBack != null){
                            displayCallBack.onAborted();
                        }
                        aecMainThread.quit();
                        isRunningAEC = false;
                        break;
                    default:
                        isRunningAEC = false;
                        break;
                }
            }
        };
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @NonNull
    private Notification buildNotification() {
        this.mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel mNotificationChannel = new NotificationChannel(
                    NOTIFY_NAME, NOTIFY_NAME, NotificationManager.IMPORTANCE_LOW);
            mNotificationManager.createNotificationChannel(mNotificationChannel);
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            mBuilder = new Notification.Builder(this, NOTIFY_NAME)
                    .setContentTitle("后台计算")
                    .setSmallIcon(R.drawable.ic_launcher_foreground);
        }
        return mBuilder.build();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    public void serviceSolveAEC(String[] numbers, String target) {
        if (!isRunningAEC && numbers.length>0 && !target.isEmpty()) {
            isRunningAEC = true;
            AECTask aecTask = new AECTask(numbers, target);
            Message message = new Message();
            message.obj = aecTask;
            message.what = AECMainThread.SOLVE;
            this.aecMainThread = new AECMainThread(displayHandler);
            setRunningState(true);
            aecMainThread.start();
            Handler aecMainHandler = aecMainThread.getHandler();
            aecMainHandler.sendMessage(message);
        }
    }

    public void serviceGenerateAEC(String boundary, String sz, String target) {
        if (!isRunningAEC) {
            isRunningAEC = true;
            String s = "";
            Matcher m = P.matcher(boundary);
            if (m.find()) s=m.group();
            Message message = new Message();
            message.obj = new String[]{s, sz, target};
            message.what = AECMainThread.GENERATE;
            this.aecMainThread = new AECMainThread(displayHandler);
            setRunningState(true);
            aecMainThread.start();
            Handler aecMainHandler = aecMainThread.getHandler();
            aecMainHandler.sendMessage(message);
        }
    }

    public void serviceStopAEC() {
        if(aecMainThread != null){
            setRunningState(false);
        }
    }

    public class AECBinder extends Binder {
        public BackgroundService getService() {
            return BackgroundService.this;
        }
    }
}