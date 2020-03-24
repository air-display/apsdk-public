//
// Created by shiontian on 11/11/2018.
//

#ifndef JNI_CLASS_LOADER_H
#define JNI_CLASS_LOADER_H
// clang-format off
#include "aps-jni.h"
// clang-format on

class jni_class_loader {
public:
  static jni_class_loader &get() {
    static jni_class_loader s_instance;
    return s_instance;
  }

  void init(JNIEnv *env) {
    jclass nci_object_clz = env->FindClass("com/virtable/airplay/NciObject");
    jclass class_clz = env->GetObjectClass(nci_object_clz);

    jclass class_loader_clz = env->FindClass("java/lang/ClassLoader");
    jmethodID mid_get_class_loader = env->GetMethodID(
        class_clz, "getClassLoader", "()Ljava/lang/ClassLoader;");
    class_loader_ = env->CallObjectMethod(nci_object_clz, mid_get_class_loader);
    class_loader_ = env->NewGlobalRef(class_loader_);

    mid_find_class_ = env->GetMethodID(class_loader_clz, "findClass",
                                       "(Ljava/lang/String;)Ljava/lang/Class;");
  }

  jclass find_class(const char *name, JNIEnv *env) {
    return static_cast<jclass>(env->CallObjectMethod(
        class_loader_, mid_find_class_, env->NewStringUTF(name)));
  }

private:
  jobject class_loader_;
  jmethodID mid_find_class_;
};

#endif // JNI_CLASS_LOADER_H
