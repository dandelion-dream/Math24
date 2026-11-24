package dandeliondream.math24.services;

public interface DisplayCallBack {
    void onProgressUpdate(AECProgress aecProgress);

    void onAnswerUpdate(String s);

    void onQuestionUpdate(AECQuetion aecAnswer);

    void onAborted();
}
