//
// Created by shiontian on 11/12/2018.
//

// clang-format off
#include <endian.h>
#include <ostream>
#include <sstream>
#include "jni_class_wrapper.h"
#include "IAirPlayHandler.h"
#include "AirPlaySession.h"
// clang-format on

IAirPlayHandler::IAirPlayHandler(JNIEnv *env)
    : jni_meta_object<IAirPlayHandler, IAirPlayHandler_cls>() {
  (void)env;
  handler_ = std::make_shared<jni_ap_handler>(this);
}

ap_handler_ptr IAirPlayHandler::get_ap_handler() { return handler_; }

void IAirPlayHandler::on_session_begin(ap_session_ptr session) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_session_begin,
                  "(Lcom/virtable/airplay/AirPlaySession;)V");
    if (mid) {
      // Create the airplay mirror session object
      jobject obj = AirPlaySession::new_jvmObject(env);
      AirPlaySession *s = AirPlaySession::attach(env, obj);
      s->set_ap_session(session);
      env->CallVoidMethod(jvm_obj_, mid, obj);
      env->DeleteLocalRef(obj);
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_mirror_stream_started");
    }
  }
}

void IAirPlayHandler::on_session_end(const uint64_t session_id) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_session_end, "(J)V");
    if (mid) {
      jlong sid = static_cast<jlong>(session_id);
      env->CallVoidMethod(jvm_obj_, mid, sid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_mirror_session_end");
    }
  }
}