package dandeliondream.math24.threads;

import android.os.Handler;
import android.os.Message;

import dandeliondream.math24.services.AECProgress;
import dandeliondream.math24.services.BackgroundService;

public class AECProgressThread extends Thread {
    private boolean isRunning;
    private final Handler showProgressHandler;

    public AECProgressThread(Handler showProgressHandler) {
        this.showProgressHandler = showProgressHandler;
    }

    public void stopProgressUpdate() {
        this.isRunning = false;
    }

    private native AECProgress getAECProgress();

    @Override
    public void run() {
        this.isRunning = true;
        while (isRunning) {
            try {
                sleep(1000);
                AECProgress aecProgress = getAECProgress();
                Message message = new Message();
                message.obj = aecProgress;
                message.what = BackgroundService.SHOW_PROGRESS;
                if(isRunning) showProgressHandler.sendMessage(message);
            } catch (InterruptedException e) {
                return;
            }
        }
    }
}
