//
// Created by shiontian on 11/11/2018.
//

#ifndef JNI_TYPE_WRAPPER_H
#define JNI_TYPE_WRAPPER_H
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

#define WRAPPER_CLASS_BEGIN(c, p)                                              \
  class c {                                                                    \
  private:                                                                     \
    static jclass clz_;                                                        \
    static jmethodID constructor_;                                             \
    JNIEnv *env_;                                                              \
    jobject obj_;                                                              \
    c(JNIEnv *env, jobject obj) : env_(env), obj_(obj) {}                      \
    static void init(JNIEnv *env) {                                            \
      if (!c::clz_) {                                                          \
        jclass clz = jni_class_loader::get().find_class(p, env);               \
        if (clz) {                                                             \
          c::clz_ = (jclass)env->NewGlobalRef(clz);                            \
        } else {                                                               \
          __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,                      \
                              "Failed to find class:" p);                      \
        }                                                                      \
      }                                                                        \
      if (c::clz_) {                                                           \
        if (!c::constructor_) {                                                \
          c::constructor_ = env->GetMethodID(c::clz_, "<init>", "()V");        \
          if (!c::constructor_) {                                              \
            __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,                    \
                                "Failed to find constructor of class:" p);     \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    }                                                                          \
                                                                               \
  public:                                                                      \
    ~c() {                                                                     \
      if (env_ && obj_)                                                        \
        env_->DeleteLocalRef(obj_);                                            \
    }                                                                          \
    static c attach(JNIEnv *env, jobject obj) {                                \
      c::init(env);                                                            \
      return c(env, obj);                                                      \
    }                                                                          \
    static c create(JNIEnv *env) {                                             \
      c::init(env);                                                            \
      jobject obj = env->NewObject(c::clz_, c::constructor_);                  \
      return c(env, obj);                                                      \
    }                                                                          \
    jobject get() { return obj_; }

#define WRAPPER_CLASS_END(c)                                                   \
  }                                                                            \
  ;                                                                            \
  jclass c::clz_ = 0;                                                          \
  jmethodID c::constructor_ = 0;

#define FIELD(x, t, s)                                                         \
public:                                                                        \
  t x() const {                                                                \
    static jfieldID fid = env_->GetFieldID(clz_, #x, #s);                      \
    return env_->Get##t##Field(obj_, fid);                                     \
  }                                                                            \
  void x(t v) {                                                                \
    static jfieldID fid = env_->GetFieldID(clz_, #x, #s);                      \
    env_->Set##t##Field(obj_, fid, v);                                         \
  }

// clang-format off

WRAPPER_CLASS_BEGIN(PlaybackInfo, "com/medialab/airplay/PlaybackInfo");
  FIELD(stallCount, Int, I);
  FIELD(duration, Double, D);
  FIELD(position, Float, F);
  FIELD(rate, Double, D);
  FIELD(readyToPlay, Boolean, Z);
  FIELD(playbackBufferEmpty, Boolean, Z);
  FIELD(playbackBufferFull, Boolean, Z);
  FIELD(playbackLikelyToKeepUp, Boolean, Z);
WRAPPER_CLASS_END(PlaybackInfo)


WRAPPER_CLASS_BEGIN(AudioControlSync, "com/medialab/airplay/AudioControlSync")
  FIELD(sequence, Short , S);
  FIELD(timestamp, Int, I);
  FIELD(currentNTPTme, Long, J);
  FIELD(nextPacketTime, Int, I);
WRAPPER_CLASS_END(AudioControlSync)


WRAPPER_CLASS_BEGIN(AudioControlRetransmit, "com/medialab/airplay/AudioControlRetransmit")
  FIELD(sequence, Short , S);
  FIELD(timestamp, Int, I);
  FIELD(lostPacketStart, Short, S);
  FIELD(lostPacketCount, Short, S);
WRAPPER_CLASS_END(AudioControlRetransmit)


WRAPPER_CLASS_BEGIN(MirroringVideoCodec, "com/medialab/airplay/MirroringVideoCodec")
  FIELD(version, Byte, B);
  FIELD(profile, Byte, B);
  FIELD(compatibility, Byte, B);
  FIELD(level, Byte, B);
  FIELD(NALLength, Byte, B);
  FIELD(spsCount, Byte, B);
  FIELD(ppsCount, Byte, B);
WRAPPER_CLASS_END(MirroringVideoCodec)

// clang-format on

#endif // JNI_TYPE_WRAPPER_H
