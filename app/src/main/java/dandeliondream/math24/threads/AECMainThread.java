package dandeliondream.math24.threads;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.text.TextUtils;

import androidx.annotation.NonNull;

import dandeliondream.math24.services.AECQuestion;
import dandeliondream.math24.services.BackgroundService;

public class AECMainThread extends HandlerThread {
    public static final int SOLVE = 0;
    public static final int GENERATE = 1;

    static {
        System.loadLibrary("dandeliondream_math");
    }

    private final Handler showResultHandler;

    public AECMainThread(Handler showResultHandler) {
        super("AEC_main");
        this.showResultHandler = showResultHandler;
    }

    private native String solveAEC(String numbers_str, String target);

    private native AECQuestion generateAEC(String boundary, String sz, String target);

    public Handler getHandler() {
        return new Handler(this.getLooper()) {
            @Override
            public void handleMessage(@NonNull Message msg) {
                switch (msg.what) {
                    case 0:
                        if (msg.obj instanceof String[]) {
                            String[] strings = (String[]) msg.obj;
                            if(strings.length == 2){
                                AECProgressThread thread = new AECProgressThread(showResultHandler);
                                thread.start();
                                String result = solveAEC(strings[0], strings[1]);
                                thread.stopProgressUpdate();
                                Message message = new Message();
                                if(!TextUtils.isEmpty(result)){  //Very Old Androids don't even have String.isEmpty().
                                    message.obj = result;
                                    message.what = BackgroundService.SHOW_ANSWER;
                                }else{
                                    message.obj = null;
                                    message.what = BackgroundService.ABORTED;
                                }
                                showResultHandler.sendMessage(message);
                            }
                        }
                    case 1:
                        if (msg.obj instanceof String[]) {
                            String[] strings = (String[]) msg.obj;
                            AECQuestion aecAnswer = generateAEC(strings[0], strings[1], strings[2]);
                            Message message = new Message();
                            message.obj = aecAnswer;
                            message.what = BackgroundService.SHOW_QUESTION;
                            showResultHandler.sendMessage(message);
                        }
                }
            }
        };
    }
}
