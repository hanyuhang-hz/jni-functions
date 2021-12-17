package com.hyh.jnifunctions;

public class Log {
	public static final boolean LOG_ON     = true;
	public static final String TAG_THREAD = "thread: ";
	public static final String TAG_TEST = "TEST";
	public static final String TAG_COMMA  = ", ";
	private static long startTime, endTime, useTime;
	
	public static void v(String tag, String text) {
		if (LOG_ON) {
			if (tag == null) tag = TAG_TEST;
			android.util.Log.v(tag, TAG_THREAD + Thread.currentThread().getId() + TAG_COMMA + text);
		}
		return;
	}
	
	public static void e(String tag, String text) {
		if (LOG_ON) {
			if (tag == null) tag = TAG_TEST;
			android.util.Log.e(tag, TAG_THREAD + Thread.currentThread().getId() + TAG_COMMA + text);
		}
		return;
	}
	
	public static void d(String tag, String text) {
		if (LOG_ON) {
			if (tag == null) tag = TAG_TEST;
			android.util.Log.d(tag, TAG_THREAD + Thread.currentThread().getId() + TAG_COMMA + text);
		}
		return;
	}
	
	public static void i(String tag, String text) {
		if (LOG_ON) {
			if (tag == null) tag = TAG_TEST;
			android.util.Log.i(tag, TAG_THREAD + Thread.currentThread().getId() + TAG_COMMA + text);
		}
		return;
	}
	
	public static void w(String tag, String text) {
		if (LOG_ON) {
			if (tag == null) tag = TAG_TEST;
			android.util.Log.w(tag, TAG_THREAD + Thread.currentThread().getId() + TAG_COMMA + text);
		}
		return;
	}

	public static void beginTime() {
		startTime = System.currentTimeMillis();
	}

	public static void endTime(String tag, String func) {
		endTime = System.currentTimeMillis();
		useTime = endTime - startTime;
		Log.d(tag, "func: " + func + " useTime: " + useTime);
	}
}
