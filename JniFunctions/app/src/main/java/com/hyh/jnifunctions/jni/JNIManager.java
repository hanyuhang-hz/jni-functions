package com.hyh.jnifunctions.jni;

import android.widget.TextView;

import com.hyh.jnifunctions.Log;
import com.hyh.jnifunctions.MainActivity;

public class JNIManager extends MainActivity {
    private static final String TAG = "HYH_JNIManager";
    public TextView tickView;
    public int hour, minute, second;
    public int mWidth = 7;
    public static int mStaicWidth = 77;

    public int StartTicks(){
        int res = 0;
        native_startTicks();
        return res;
    }

    public int StopTicks(){
        int res = 0;
        native_stopTicks();
        return res;
    }

    private void updateTimer() {
        ++second;
        if (second >= 60) {
            ++minute;
            second -= 60;
            if (minute >= 60) {
                ++hour;
                minute -= 60;
            }
        }
        Log.d(TAG, "updateTimer hour:" + hour +
                " minute:" + minute + " second" + second);
        //TODO crash done: runOnUiThread:need extends Mainactivity
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String ticks = hour + ":" + minute + ":" + second;
                tickView.setText(ticks);
            }
        });
    }

    //10.11:直接抓log验证，不做界面了
    public void StringOperations(){
        native_stringOperations();
    }

    public void ArrayOperations(){
        native_arrayOperations();
    }

    public void ObjectOperations(){
        native_objectOperations();
    }

    public void ClassAndInterfaceOperations(){
        native_classAndInterfaceOperations();
    }

    public void Exceptions(){
        native_exceptions();
    }

    public static void ExcepitonCallback(){
        Log.d(TAG,"ExcepitonCallback");
        //int a = 20 / 0;         //if test Native_Exceptions,open comment
    }

    public static void NormalCallback(){
        Log.d(TAG,"NormalCallback");
        //int a = 10 / 0;
    }

    public void GlobalAndLocalReferences(){
        native_globalAndLocalReferences();
    }

    public void InstanceFieldAccess(){
        native_instanceField();
    }

    public void StaticFieldAccess(){
        native_staticField();
    }

    public void InstanceMethodCallback(){
        Log.d(TAG,"InstanceMethodCallback!!!");
    }
    public void InstanceMethodCalls(){
        native_instanceMethod();
    }

    public static void StaticMethodCallback(){
        Log.d(TAG,"StaticMethodCallback!!!");
    }
    public void StaticMethodCalls(){
        native_staticMethod();
    }

    public void JavaVMInterface(){
        native_javaVMInterface();
    }

    public void MonitorOperations(){
        native_monitorOperations();
    }

    public native void native_startTicks();
    public native void native_stopTicks();
    public native void native_stringOperations();
    public native void native_arrayOperations();
    public native void native_objectOperations();
    public native void native_classAndInterfaceOperations();
    public native void native_exceptions();
    public native void native_globalAndLocalReferences();
    public native void native_instanceField();
    public native void native_staticField();
    public native void native_instanceMethod();
    public native void native_staticMethod();
    public native void native_javaVMInterface();
    public native void native_monitorOperations();

    static {
        try {
            System.loadLibrary("test_jni");
        } catch (UnsatisfiedLinkError ule) {
            Log.e(TAG, ule.toString());
        }
    }
}
