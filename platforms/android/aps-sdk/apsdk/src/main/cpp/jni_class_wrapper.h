//
// Created by shiontian on 11/11/2018.
//

#ifndef JNI_CLASS_WRAPPER_H
#define JNI_CLASS_WRAPPER_H
#include <android/log.h>
#include <jni.h>
#include <jni_class_loader.h>
#include <mutex>

template<const char *CLS> class jni_class_meta {
protected:
  static jclass get_class(JNIEnv *env) {
    static jclass clz_ = 0;
    if (clz_)
      return clz_;
    clz_ = jni_class_loader::get().find_class(CLS, env);
    if (clz_) {
      clz_ = (jclass) env->NewGlobalRef(clz_);
    } else {
      __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to find class: %s", CLS);
    }
    return clz_;
  }

  static jmethodID get_constructor(JNIEnv *env) {
    static jmethodID constructor_ = 0;
    if (constructor_)
      return constructor_;
    jclass clz = get_class(env);
    if (clz) {
      constructor_ = env->GetMethodID(clz, "<init>", "()V");
      if (!constructor_) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Failed to find constructor of class:%s", CLS);
      }
    }
    return constructor_;
  }
};

#define GET_METHOD_ID(name, sig)                                               \
  static jmethodID mid = env->GetMethodID(get_class(env), #name, sig);         \
  if (!mid)                                                                    \
    mid = env->GetMethodID(get_class(env), #name, sig);                        \
  ;

template<typename T, const char *CLS>
class jni_meta_object : public jni_class_meta<CLS> {
public:
  static T *attach(JNIEnv *env, jobject o) {
    T *p = new (std::nothrow)T(env);
    if (p) {
      p->jvm_obj_ = env->NewGlobalRef(o);
    }
    return p;
  }

  static void destroy(JNIEnv *env, jobject o) {
    T *p = T::get(env, o);
    jobject ref = p->jvm_obj_;
    delete (T *) ((void *) (p));
    if (ref) {
      env->DeleteGlobalRef(ref);
    }
  }

protected:
  jobject jvm_obj_;
};

typedef jint Int;
typedef jshort Short;
typedef jlong Long;
typedef jdouble Double;
typedef jfloat Float;
typedef jboolean Boolean;
typedef jbyte Byte;
typedef jobject Object;

#define WRAPPER_CLASS_BEGIN(c, p)                                              \
  static const char c##_cls[] = p;                                             \
  class c : public jni_class_meta<c##_cls> {                                   \
  public:                                                                      \
    ~c() {}                                                                    \
    static c attach(JNIEnv *env, jobject obj) { return c(env, obj); }          \
    static c create(JNIEnv *env) {                                             \
      jobject obj =                                                            \
          env->NewObject(jni_class_meta<c##_cls>::get_class(env),              \
                         jni_class_meta<c##_cls>::get_constructor(env));       \
      return c(env, obj);                                                      \
    }                                                                          \
    jobject get() { return obj_; }                                             \
                                                                               \
  protected:                                                                   \
    JNIEnv *env_;                                                              \
    jobject obj_;                                                              \
    c(JNIEnv *env, jobject obj) : env_(env), obj_(obj) {}

#define WRAPPER_CLASS_END()                                                    \
  }                                                                            \
  ;

#define FIELD(x, t, s)                                                         \
public:                                                                        \
  t x() const {                                                                \
    static jfieldID fid = env_->GetFieldID(get_class(env_), #x, s);            \
    return env_->Get##t##Field(obj_, fid);                                     \
  }                                                                            \
  void x(t v) {                                                                \
    static jfieldID fid = env_->GetFieldID(get_class(env_), #x, s);            \
    env_->Set##t##Field(obj_, fid, v);                                         \
  }

class String {
public:
  static String attach(JNIEnv *env, jstring obj, bool auto_release = true) {
    return String(env, obj, auto_release);
  }

  static String fromUTF8(JNIEnv *env, const char *s) {
    jstring obj = env->NewStringUTF(s);
    return String(env, obj, true);
  }

  jstring get() { return obj_; }

  ~String() {}

protected:
  JNIEnv *env_;
  jstring obj_;

  String(JNIEnv *env, jstring obj, bool auto_release) : env_(env), obj_(obj) {}
};

template<typename T> class LocalJvmObject : public T {
public:
  LocalJvmObject(const T &other) : T(other) {}

  ~LocalJvmObject() {
    if (this->env_ && this->obj_)
      this->env_->DeleteLocalRef(this->obj_);
  }

private:
  LocalJvmObject() = delete;
  LocalJvmObject(const LocalJvmObject &other) = delete;
  LocalJvmObject &operator=(const LocalJvmObject &other) = delete;
};

#define SHORT_FIELD(x) FIELD(x, Short, "S")
#define INT_FIELD(x) FIELD(x, Int, "I")
#define LONG_FIELD(x) FIELD(x, Long, "J")
#define DOUBLE_FIELD(x) FIELD(x, Double, "D")
#define FLOAT_FIELD(x) FIELD(x, Float, "F")
#define BOOLEAN_FIELD(x) FIELD(x, Boolean, "Z")
#define STRING_FIELD(x) FIELD(x, Object, "Ljava/lang/String;")
#define OBJECT_FIELD(x, s) FIELD(x, Object, s)

// clang-format off

WRAPPER_CLASS_BEGIN(PlaybackInfo, "com/virtable/airplay/PlaybackInfo")
  INT_FIELD(stallCount);
  DOUBLE_FIELD(duration);
  FLOAT_FIELD(position);
  DOUBLE_FIELD(rate);
  BOOLEAN_FIELD(readyToPlay);
  BOOLEAN_FIELD(playbackBufferEmpty);
  BOOLEAN_FIELD(playbackBufferFull);
  BOOLEAN_FIELD(playbackLikelyToKeepUp);
WRAPPER_CLASS_END()

/*
WRAPPER_CLASS_BEGIN(AudioControlSync, "com/virtable/airplay/AudioControlSync")
  SHORT_FIELD(sequence);
  INT_FIELD(timestamp);
  LONG_FIELD(currentNTPTme);
  INT_FIELD(nextPacketTime);
WRAPPER_CLASS_END()
*/

/*
WRAPPER_CLASS_BEGIN(AudioControlRetransmit, "com/virtable/airplay/AudioControlRetransmit")
  SHORT_FIELD(sequence);
  INT_FIELD(timestamp);
  SHORT_FIELD(lostPacketStart);
  SHORT_FIELD(lostPacketCount);
WRAPPER_CLASS_END()
*/

/*
WRAPPER_CLASS_BEGIN(AirPlayConfigDisplay, "com/virtable/airplay/AirPlayConfigDisplay")
  INT_FIELD(width);
  INT_FIELD(height);
WRAPPER_CLASS_END()
*/

WRAPPER_CLASS_BEGIN(AirPlayConfig, "com/virtable/airplay/AirPlayConfig")
  STRING_FIELD(name);
  BOOLEAN_FIELD(publishService);
  STRING_FIELD(macAddress);
  STRING_FIELD(deviceID);
  STRING_FIELD(model);
  STRING_FIELD(sourceVersion);
  STRING_FIELD(pi);
  STRING_FIELD(pk);
  INT_FIELD(vv);
  INT_FIELD(features);
  INT_FIELD(statusFlag);
  OBJECT_FIELD(display, "com/virtable/airplay/AirPlayConfigDisplay");
WRAPPER_CLASS_END()

// clang-format on

#endif // JNI_CLASS_WRAPPER_H
