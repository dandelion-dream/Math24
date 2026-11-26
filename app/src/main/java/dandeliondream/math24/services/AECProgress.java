package dandeliondream.math24.services;

import android.annotation.SuppressLint;

public class AECProgress {
   private final long roundTotal;
    private final long roundFinished;
    private final long bucketTotal;
    private final long bucketFilled;
    private final long combTotal;
    private final long combFinished;

    public AECProgress(long roundTotal, long roundFinished, long bucketTotal, long bucketFilled, long combTotal, long combFinished){
        this.roundTotal=roundTotal;
        this.roundFinished=roundFinished;
        this.bucketTotal=bucketTotal;
        this.bucketFilled=bucketFilled;
        this.combTotal=combTotal;
        this.combFinished=combFinished;
    }

    public int getPercentage(){
        return (int)Math.floor((double) (100 * combFinished) /combTotal);
    }

    @SuppressLint("DefaultLocale")
    public String toStringLong() {
        double d = (double) (100 * combFinished) /combTotal;
        return String.format("Rounds: %d/%d\nBuckets Filled: %d/%d\nNode Combined in the Bucket: %d/%d\nCombination Progress: %.4f%%"
                                ,roundFinished,roundTotal,bucketFilled,bucketTotal,combFinished,combTotal,d);
    }

    @SuppressLint("DefaultLocale")
    public String toStringShort(){
        double d = (double) (100 * combFinished) /combTotal;
        return String.format("%d/%d %d/%d %d/%d %.4f%%"
                ,roundFinished,roundTotal,bucketFilled,bucketTotal,combFinished,combTotal,d);
    }
}
