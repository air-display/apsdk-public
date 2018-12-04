//
// Created by shiontian on 11/11/2018.
//

#ifndef JNI_CLASS_WRAPPER_H
#define JNI_CLASS_WRAPPER_H
#include <android/log.h>
#include <jni.h>
#include <jni_class_loader.h>
#include <mutex>

typedef jint Int;
typedef jshort Short;
typedef jlong Long;
typedef jdouble Double;
typedef jfloat Float;
typedef jboolean Boolean;
typedef jbyte Byte;
typedef jobject Object;

#define WRAPPER_CLASS_BEGIN(c, p)                                              \
  class c {                                                                    \
  private:                                                                     \
    JNIEnv *env_;                                                              \
    jobject obj_;                                                              \
    c(JNIEnv *env, jobject obj) : env_(env), obj_(obj) {}                      \
    static jclass get_class(JNIEnv *env) {                                     \
      static jclass clz_ = 0;                                                  \
      if (clz_)                                                                \
        return clz_;                                                           \
      clz_ = jni_class_loader::get().find_class(p, env);                       \
      if (clz_) {                                                              \
        clz_ = (jclass)env->NewGlobalRef(clz_);                                \
      } else {                                                                 \
        __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,                        \
                            "Failed to find class:" p);                        \
      }                                                                        \
      return clz_;                                                             \
    }                                                                          \
    static jmethodID get_constructor(JNIEnv *env) {                            \
      static jmethodID constructor_ = 0;                                       \
      if (constructor_)                                                        \
        return constructor_;                                                   \
      jclass clz = get_class(env);                                             \
      if (clz) {                                                               \
        constructor_ = env->GetMethodID(clz, "<init>", "()V");                 \
        if (!constructor_) {                                                   \
          __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,                      \
                              "Failed to find constructor of class:" p);       \
        }                                                                      \
      }                                                                        \
      return constructor_;                                                     \
    }                                                                          \
                                                                               \
  public:                                                                      \
    ~c() {                                                                     \
      if (env_ && obj_)                                                        \
        env_->DeleteLocalRef(obj_);                                            \
    }                                                                          \
    static c attach(JNIEnv *env, jobject obj) { return c(env, obj); }          \
    static c create(JNIEnv *env) {                                             \
      jobject obj = env->NewObject(get_class(env), get_constructor(env));      \
      return c(env, obj);                                                      \
    }                                                                          \
    jobject get() { return obj_; }

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

#define SHORT_FIELD(x) FIELD(x, Short, "S")
#define INT_FIELD(x) FIELD(x, Int, "I")
#define LONG_FIELD(x) FIELD(x, Long, "J")
#define DOUBLE_FIELD(x) FIELD(x, Double, "D")
#define FLOAT_FIELD(x) FIELD(x, Float, "F")
#define BOOLEAN_FIELD(x) FIELD(x, Boolean, "Z")
#define STRING_FIELD(x) FIELD(x, Object, "Ljava/lang/String;")
#define OBJECT_FIELD(x, s) FIELD(x, Object, s)

// clang-format off

WRAPPER_CLASS_BEGIN(PlaybackInfo, "com/medialab/airplay/PlaybackInfo")
  INT_FIELD(stallCount);
  DOUBLE_FIELD(duration);
  FLOAT_FIELD(position);
  DOUBLE_FIELD(rate);
  BOOLEAN_FIELD(readyToPlay);
  BOOLEAN_FIELD(playbackBufferEmpty);
  BOOLEAN_FIELD(playbackBufferFull);
  BOOLEAN_FIELD(playbackLikelyToKeepUp);
WRAPPER_CLASS_END()


WRAPPER_CLASS_BEGIN(AudioControlSync, "com/medialab/airplay/AudioControlSync")
  SHORT_FIELD(sequence);
  INT_FIELD(timestamp);
  LONG_FIELD(currentNTPTme);
  INT_FIELD(nextPacketTime);
WRAPPER_CLASS_END()


WRAPPER_CLASS_BEGIN(AudioControlRetransmit, "com/medialab/airplay/AudioControlRetransmit")
  SHORT_FIELD(sequence);
  INT_FIELD(timestamp);
  SHORT_FIELD(lostPacketStart);
  SHORT_FIELD(lostPacketCount);
WRAPPER_CLASS_END()


WRAPPER_CLASS_BEGIN(MirroringVideoCodec, "com/medialab/airplay/MirroringVideoCodec")
  SHORT_FIELD(version);
  SHORT_FIELD(profile);
  SHORT_FIELD(compatibility);
  SHORT_FIELD(level);
  SHORT_FIELD(NALLength);
  SHORT_FIELD(spsCount);
  SHORT_FIELD(ppsCount);
WRAPPER_CLASS_END()

WRAPPER_CLASS_BEGIN(AirPlayConfigDisplay, "com/medialab/airplay/AirPlayConfigDisplay")
  INT_FIELD(width);
  INT_FIELD(height);
WRAPPER_CLASS_END()

WRAPPER_CLASS_BEGIN(AirPlayConfig, "com/medialab/airplay/AirPlayConfig")
  STRING_FIELD(name);
  STRING_FIELD(deviceID);
  STRING_FIELD(model);
  STRING_FIELD(sourceVersion);
  STRING_FIELD(pi);
  STRING_FIELD(pk);
  STRING_FIELD(macAddress);
  INT_FIELD(vv);
  INT_FIELD(features);
  INT_FIELD(statusFlag);
  OBJECT_FIELD(display, "com/medialab/airplay/AirPlayConfigDisplay");
WRAPPER_CLASS_END()

// clang-format on

#endif // JNI_CLASS_WRAPPER_H
