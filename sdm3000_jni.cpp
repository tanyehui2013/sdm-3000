#include <jni.h>
#include <iostream>
#include <vector>
#include <cstring>

// SDM-3000 协议常量
const unsigned char STX = 0x02;
const unsigned char ETX = 0x03;
const unsigned char ACK = 0x06;
const unsigned char NAK = 0x15;
const unsigned char ENQ = 0x05;

// 客户需要的5条指令
const unsigned char CMD_RESET = 0x30;           // 重置
const unsigned char CMD_READ_STATUS = 0x31;     // 读状态
const unsigned char CMD_DIAGNOSTICS = 0x32;     // 诊断
const unsigned char CMD_LAST_STATUS = 0x34;     // 最后状态
const unsigned char CMD_MULTI_DISPENSE = 0x3A;  // 多钞箱出钞

// 计算BCC异或校验
unsigned char calculateBCC(const unsigned char* data, int start, int length) {
    unsigned char bcc = 0;
    for (int i = start; i < start + length; i++) {
        bcc ^= data[i];
    }
    return bcc;
}

// 模拟串口发送函数 (在实际硬件上会替换为真实串口写操作)
bool sendCommandFrame(unsigned char cmd, const unsigned char* data, int dataLen) {
    std::cout << "[JNI LOG] 准备发送命令: 0x" << std::hex << (int)cmd << std::dec << std::endl;
    std::cout << "[JNI LOG] 数据长度: " << dataLen << " 字节" << std::endl;
    
    // 1. 构建帧：STX + LEN + CMD + DATA + ETX + BCC
    int frameLen = 5 + dataLen;
    std::vector<unsigned char> frame(frameLen);
    
    frame[0] = STX;                     // STX
    frame[1] = (unsigned char)(1 + dataLen); // LEN (1字节CMD + 数据长度)
    frame[2] = cmd;                     // CMD
    
    if (dataLen > 0 && data != nullptr) {
        memcpy(&frame[3], data, dataLen); // DATA
    }
    
    frame[3 + dataLen] = ETX; // ETX
    // BCC = LEN ^ CMD ^ DATA[0] ^ ... ^ DATA[N] ^ ETX
    frame[4 + dataLen] = calculateBCC(frame.data(), 1, 2 + dataLen); // BCC
    
    // 2. 打印出构建的帧用于验证
    std::cout << "[JNI LOG] 构建的帧数据: ";
    for (int i = 0; i < frameLen; i++) {
        printf("%02X ", frame[i]);
    }
    std::cout << std::endl;
    
    // 3. 这里是模拟发送成功
    std::cout << "[JNI LOG] *** 模拟: 帧已通过串口发送至设备 ***" << std::endl;
    return true;
}

// ============= JNI 函数实现 =============
extern "C" {

// 连接设备 (模拟)
JNIEXPORT jboolean JNICALL
Java_com_example_sdm3000_SDM3000JNI_connect(
    JNIEnv* env, jobject thiz, jstring devicePath, jint baudRate) {
    
    const char* path = env->GetStringUTFChars(devicePath, nullptr);
    std::cout << "[JNI LOG] >>> 调用 connect()" << std::endl;
    std::cout << "[JNI LOG] 设备路径: " << path << ", 波特率: " << baudRate << std::endl;
    env->ReleaseStringUTFChars(devicePath, path);
    
    // 模拟连接成功
    std::cout << "[JNI LOG] *** 模拟: 串口连接成功 ***" << std::endl;
    return JNI_TRUE;
}

// 重置命令 0x30
JNIEXPORT jboolean JNICALL
Java_com_example_sdm3000_SDM3000JNI_reset(JNIEnv* env, jobject thiz) {
    std::cout << "\n[JNI LOG] >>> 调用 reset() [命令 0x30]" << std::endl;
    bool success = sendCommandFrame(CMD_RESET, nullptr, 0);
    return success ? JNI_TRUE : JNI_FALSE;
}

// 读状态命令 0x31
JNIEXPORT jbyteArray JNICALL
Java_com_example_sdm3000_SDM3000JNI_readStatus(JNIEnv* env, jobject thiz) {
    std::cout << "\n[JNI LOG] >>> 调用 readStatus() [命令 0x31]" << std::endl;
    
    if (!sendCommandFrame(CMD_READ_STATUS, nullptr, 0)) {
        return nullptr;
    }
    
    // 模拟返回一个假的设备状态数据 (例如 12 字节)
    unsigned char fakeStatus[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};
    int dataSize = sizeof(fakeStatus);
    jbyteArray result = env->NewByteArray(dataSize);
    env->SetByteArrayRegion(result, 0, dataSize, (const jbyte*)fakeStatus);
    
    std::cout << "[JNI LOG] 返回模拟状态数据，长度: " << dataSize << " 字节" << std::endl;
    return result;
}

// 多钞箱出钞命令 0x3A
JNIEXPORT jboolean JNICALL
Java_com_example_sdm3000_SDM3000JNI_multiDispense(
    JNIEnv* env, jobject thiz, jbyteArray counts) {
    
    std::cout << "\n[JNI LOG] >>> 调用 multiDispense() [命令 0x3A]" << std::endl;
    
    // 1. 从Java数组获取数据
    jsize len = env->GetArrayLength(counts);
    if (len != 8) {
        std::cerr << "[JNI LOG] 错误: 需要8个字节的出钞数量，但收到: " << len << std::endl;
        return JNI_FALSE;
    }
    
    jbyte* countsData = env->GetByteArrayElements(counts, nullptr);
    
    // 2. 打印出每个钞箱的出钞张数
    std::cout << "[JNI LOG] 各钞箱出钞张数: ";
    for (int i = 0; i < 8; i++) {
        std::cout << (int)countsData[i] << " ";
    }
    std::cout << std::endl;
    
    // 3. 发送命令
    bool success = sendCommandFrame(CMD_MULTI_DISPENSE, 
                                   (const unsigned char*)countsData, 8);
    
    env->ReleaseByteArrayElements(counts, countsData, 0);
    return success ? JNI_TRUE : JNI_FALSE;
}

// 其他命令的模拟实现 (结构类似)
JNIEXPORT jbyteArray JNICALL
Java_com_example_sdm3000_SDM3000JNI_diagnostics(JNIEnv* env, jobject thiz) {
    std::cout << "\n[JNI LOG] >>> 调用 diagnostics() [命令 0x32]" << std::endl;
    sendCommandFrame(CMD_DIAGNOSTICS, nullptr, 0);
    // 返回模拟诊断数据
    unsigned char fakeData[] = {0xAA, 0xBB, 0xCC};
    jbyteArray result = env->NewByteArray(3);
    env->SetByteArrayRegion(result, 0, 3, (const jbyte*)fakeData);
    return result;
}

JNIEXPORT jbyteArray JNICALL
Java_com_example_sdm3000_SDM3000JNI_lastStatus(JNIEnv* env, jobject thiz) {
    std::cout << "\n[JNI LOG] >>> 调用 lastStatus() [命令 0x34]" << std::endl;
    sendCommandFrame(CMD_LAST_STATUS, nullptr, 0);
    // 返回模拟最后状态数据
    unsigned char fakeData[] = {0x11, 0x22, 0x33, 0x44};
    jbyteArray result = env->NewByteArray(4);
    env->SetByteArrayRegion(result, 0, 4, (const jbyte*)fakeData);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_com_example_sdm3000_SDM3000JNI_disconnect(JNIEnv* env, jobject thiz) {
    std::cout << "\n[JNI LOG] >>> 调用 disconnect()" << std::endl;
    std::cout << "[JNI LOG] *** 模拟: 串口连接已关闭 ***" << std::endl;
    return JNI_TRUE;
}

}
