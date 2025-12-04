package com.example.sdm3000;

public class SDM3000JNI {
    static {
        System.loadLibrary("sdm3000_jni");
    }
    
    // JNI方法声明
    public native boolean connect(String devicePath, int baudRate);
    public native boolean disconnect();
    public native boolean reset();
    public native byte[] readStatus();
    public native byte[] diagnostics();
    public native byte[] lastStatus();
    public native boolean multiDispense(byte[] counts);
    
    // 便捷方法
    public boolean connect(String devicePath) {
        return connect(devicePath, 9600);
    }
}
