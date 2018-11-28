//
// Created by shiontian on 11/11/2018.
//

#ifndef APS_SDK_NCI_OBJECT_H
#define APS_SDK_NCI_OBJECT_H
#include <aps-jni.h>

class nci_core {
  static JavaVM *vm_;
  static jclass clz_;
  static jfieldID field_NicPtr_;

public:
  static void initialize(JavaVM *vm, JNIEnv *env);

  static JavaVM *get_JavaVM();

  static jlong get_nciPtr(JNIEnv *env, jobject o);

  static void throw_null_exception(JNIEnv *env);

private:
  nci_core() = delete;
};

template <typename T> class nci_object {
public:
  static JavaVM *get_JavaVM() { return nci_core::get_JavaVM(); }

  static T *create(JNIEnv *env, jobject o) {
    T *p = new T(env);
    if (p) {
      p->obj_this_ = env->NewGlobalRef(o);
    }
    return p;
  }

  static T *get(JNIEnv *env, jobject o) {
    return (T *)(void *)(nci_core::get_nciPtr(env, o));
  }

  static void destroy(JNIEnv *env, jobject o) {
    T *p = T::get(env, o);
    if (0 == p) {
      return nci_core::throw_null_exception(env);
    }
    jobject ref = p->obj_this_;
    delete (T *)((void *)(p));
    if (ref) {
      env->DeleteGlobalRef(ref);
    }
  }

protected:
  jobject obj_this_;
};

#endif // APS_SDK_NCI_OBJECT_H
