cmake_minimum_required(VERSION 3.10)
project(sdm3000_jni)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 创建共享库 (JNI库)
add_library(sdm3000_jni SHARED sdm3000_jni.cpp)

# 在macOS/Linux上，库文件会生成为 libsdm3000_jni.so
# 在Windows上，库文件会生成为 sdm3000_jni.dll
