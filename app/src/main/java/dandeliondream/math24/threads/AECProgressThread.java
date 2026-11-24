package dandeliondream.math24.threads;

import android.os.Handler;
import android.os.Message;

import dandeliondream.math24.services.AECProgress;
import dandeliondream.math24.services.BackgroundService;

public class AECProgressThread extends Thread {
    private boolean isRunning;
    private Handler showProgressHandler;

    public AECProgressThread(Handler showProgressHandler) {
        this.showProgressHandler = showProgressHandler;
    }

    public void mstop() {
        this.isRunning = false;
    }

    private native AECProgress getAECProgress();

    @Override
    public void run() {
        this.isRunning = true;
        while (isRunning) {
            try {
                sleep(1000);
            } catch (InterruptedException e) {
                return;
            }

            AECProgress aecProgress = getAECProgress();
            Message message = new Message();
            message.obj = aecProgress;
            message.what = BackgroundService.SHOW_PROGRESS;
            showProgressHandler.sendMessage(message);
        }
    }
}
