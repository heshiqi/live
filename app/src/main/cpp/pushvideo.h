//
// Created by heshiqi on 17/7/4.
//

#ifndef LIVEPUSH_TEST_H
#define LIVEPUSH_TEST_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
Java_com_example_heshiqi_livepushdome_FFmpeg_init(JNIEnv *env, jobject instance);

JNIEXPORT jint JNICALL
Java_com_example_heshiqi_livepushdome_FFmpeg_pushVideoStream(JNIEnv *env, jobject instance,
                                                                   jstring srcFilePath_,
                                                                   jstring url_);
JNIEXPORT void JNICALL
Java_com_example_heshiqi_livepushdome_FFmpeg_exit(JNIEnv *env, jobject instance);
#ifdef __cplusplus
}
#endif
#endif //LIVEPUSH_TEST_H
