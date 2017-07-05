package com.example.heshiqi.livepushdome;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity implements FFmpeg.OnNativeCallBack{

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    private TextView textView;
    private String videoPath= Environment.getExternalStorageDirectory()+"/abc.mp4";
    FFmpeg fFmpeg=new FFmpeg();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView=(TextView)findViewById(R.id.sample_text);
        // Example of a call to a native method
        File file=new File(videoPath);
        fFmpeg.setCallBack(this);
        findViewById(R.id.btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(){
                    @Override
                    public void run() {
                        fFmpeg.init();
                        fFmpeg.pushVideoStream(videoPath,"rtmp://ws-publishlive.autohome.com.cn/event/8758_10002");
                    }
                }.start();

            }
        });
        findViewById(R.id.btn2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                fFmpeg.exit();
            }
        });
    }


    @Override
    public void pushProsess(final int arg0) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                textView.setText( String.format("Send %8d video frames to output URL", arg0));
            }
        });
    }

    @Override
    protected void onDestroy() {
        fFmpeg.exit();
        super.onDestroy();
    }
}
