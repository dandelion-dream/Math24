package dandeliondream.math24.ui;

import android.annotation.SuppressLint;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import dandeliondream.math24.R;
import dandeliondream.math24.databinding.ActivityAecBinding;
import dandeliondream.math24.services.AECProgress;
import dandeliondream.math24.services.AECQuestion;
import dandeliondream.math24.services.BackgroundService;
import dandeliondream.math24.services.DisplayCallBack;

public class AECActivity extends AppCompatActivity implements DisplayCallBack {

    @SuppressLint("UseSwitchCompatOrMaterialCode")
    private ClipboardManager clipAEC;
    private BackgroundService s;
    private boolean bound = false;
    private ServiceConnection connAEC;

    private EditText editTextRank, editTextValues;
    private TextView textViewOutput;

    private ProgressBar progressBarAEC;

    private AECQuestion aecQuetion;

    @Override
    public void onAnswerUpdate(String s) {
        this.textViewOutput.setText(s);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            this.progressBarAEC.setProgress(0,false);
        }else{
            this.progressBarAEC.setProgress(0);
        }
    }

    @Override
    public void onQuestionUpdate(AECQuestion aecQuetion) {
        this.aecQuetion = aecQuetion;
        String[] strings = this.aecQuetion.aecQuestion.split("=");
        this.editTextValues.setText(strings[0]);
        this.editTextRank.setText(strings[1]);
        this.textViewOutput.setText(strings[0]);
    }

    @Override
    public void onProgressUpdate(AECProgress aecProgress) {
        this.textViewOutput.setText(aecProgress.toStringLong());
        int progressInt = (int) Math.floor(aecProgress.getPercentage());
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            this.progressBarAEC.setProgress(progressInt,true);
        }else{
            this.progressBarAEC.setProgress(progressInt);
        }
    }

    @SuppressLint("SetTextI18n")
    @Override
    public void onAborted() {
        this.textViewOutput.setText("Current Task Aborted");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            this.progressBarAEC.setProgress(0,false);
        }else{
            this.progressBarAEC.setProgress(0);
        }
    }

    @Override
    protected void onDestroy() {
        s.displayCallBack = null;
        if (bound) {
            unbindService(connAEC);
        }
        super.onDestroy();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityAecBinding binding = ActivityAecBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        this.editTextRank = binding.editTextRank;
        EditText editTextNumberOfOperands = binding.editTextNumberOfOperands;
        this.editTextValues = binding.editTextValues;
        this.progressBarAEC = binding.progressBarAEC;

        Button buttonSolveAEC = binding.buttonSolveAEC;
        Button buttonGenerateAEC = binding.buttonGenerateAEC;
        Button buttonStopAEC = binding.buttonStopAEC;
        Button buttonShowAnswer = binding.buttonShowAnswer;
        Button buttonClear = binding.buttonClear;

        @SuppressLint("UseSwitchCompatOrMaterialCode") Switch switchPureRandomGenerate = binding.switchPureRandomGenerate;
        @SuppressLint("UseSwitchCompatOrMaterialCode") Switch switchEnableExp = binding.switchEnableExp;
        @SuppressLint("UseSwitchCompatOrMaterialCode") Switch switchAllowNotUsingAllValues = binding.switchAllowNotUsingAllValues;
        @SuppressLint("UseSwitchCompatOrMaterialCode") Switch switchAllowValuesUseMoreTime = binding.switchAllowValuesUseMoreTime;

        textViewOutput = binding.textViewOutput;

        Intent intent = new Intent(this, BackgroundService.class);
        startService(intent);

        clipAEC = (ClipboardManager) getSystemService(
                Context.CLIPBOARD_SERVICE);

        connAEC = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName name, IBinder b) {
                BackgroundService.AECBinder binder = (BackgroundService.AECBinder) b;
                s = binder.getService();
                s.displayCallBack = AECActivity.this;
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                s = null;
            }
        };
        bound = bindService(intent, connAEC, BIND_AUTO_CREATE);

        buttonSolveAEC.setOnClickListener(v -> {
            String[] input = editTextValues.getText().toString().split(" ");
            String target = editTextRank.getText().toString();
            s.serviceSolveAEC(input, target);
        });

        buttonGenerateAEC.setOnClickListener(v -> s.serviceGenerateAEC(
                editTextValues.getText().toString(),
                editTextNumberOfOperands.getText().toString(),
                editTextRank.getText().toString()));

        buttonStopAEC.setOnClickListener(v -> s.serviceStopAEC());

        textViewOutput.setOnClickListener(v -> {
            ClipData result = ClipData.newPlainText(
                    getString(R.string.result), textViewOutput.getText());
            clipAEC.setPrimaryClip(result);
            Toast.makeText(getApplicationContext()
                    , R.string.copied, Toast.LENGTH_SHORT).show();
        });

        buttonShowAnswer.setOnClickListener(v -> {
            if (this.aecQuetion != null){
                this.textViewOutput.setText(this.aecQuetion.aecAnswer);
            }
        });

        buttonClear.setOnClickListener(v -> {
            editTextRank.setText("");
            editTextNumberOfOperands.setText("");
            editTextValues.setText("");
        });
    }
}