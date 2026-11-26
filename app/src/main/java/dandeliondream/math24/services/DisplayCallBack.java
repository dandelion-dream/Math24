package dandeliondream.math24.services;

public interface DisplayCallBack {
    void onProgressUpdate(AECProgress aecProgress);
    void onAnswerUpdate(String s);
    void onQuestionUpdate(AECQuestion aecAnswer);
    void onAborted();
}
