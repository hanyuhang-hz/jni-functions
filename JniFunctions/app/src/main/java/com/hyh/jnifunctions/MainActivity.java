package com.hyh.jnifunctions;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

import com.hyh.jnifunctions.jni.JNIManager;

public class MainActivity extends Activity {
    private static final String TAG = "HYH_MainActivity";
    private JNIManager mHellolibsJNI = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mHellolibsJNI = new JNIManager();
        mHellolibsJNI.tickView = (TextView) findViewById(R.id.tickView);
  }

    @Override
    protected void onResume() {
        super.onResume();
        mHellolibsJNI.hour = mHellolibsJNI.minute = mHellolibsJNI.second = 0;
        mHellolibsJNI.StartTicks();
        mHellolibsJNI.StringOperations();
        mHellolibsJNI.ArrayOperations();
        mHellolibsJNI.ObjectOperations();
        mHellolibsJNI.ClassAndInterfaceOperations();
        mHellolibsJNI.Exceptions();
        mHellolibsJNI.GlobalAndLocalReferences();
        mHellolibsJNI.InstanceFieldAccess();
        mHellolibsJNI.StaticFieldAccess();
        mHellolibsJNI.InstanceMethodCalls();
        mHellolibsJNI.StaticMethodCalls();
        mHellolibsJNI.JavaVMInterface();
        mHellolibsJNI.MonitorOperations();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mHellolibsJNI.StopTicks();
    }

}
