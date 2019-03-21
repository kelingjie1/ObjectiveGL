#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_objectivegl_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello Objective-GL";
    return env->NewStringUTF(hello.c_str());
}
