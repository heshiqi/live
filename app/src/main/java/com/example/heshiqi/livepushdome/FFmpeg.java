package com.example.heshiqi.livepushdome;

import android.util.Log;

import java.util.Objects;

/**
 * Created by heshiqi on 17/7/5.
 */

public class FFmpeg {

    public interface OnNativeCallBack{
        void pushProsess(int arg0);
    }

    private OnNativeCallBack callBack;

    public void setCallBack(OnNativeCallBack callBack) {
        this.callBack = callBack;
    }

    public native void init();

    public native int pushVideoStream(String srcFilePath, String url);

    public native void exit();

    public void nativeCall(int arg){
        if(callBack!=null){
            callBack.pushProsess(arg);
        }
    }
}
