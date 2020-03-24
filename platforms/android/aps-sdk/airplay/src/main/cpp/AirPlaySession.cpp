// clang-format off
#include <dlfcn.h>
#include <string>
#include "AirPlaySession.h"
// clang-format on

DEFINE_NCI_METHODS(AirPlaySession);

extern "C" JNIEXPORT jint JNICALL
Java_com_virtable_airplay_AirPlaySession_nciGetSessionType(
    JNIEnv *env, jobject instance) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    return p->get_session_type();
  }
  return 0;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_virtable_airplay_AirPlaySession_nciGetSessionId(
    JNIEnv *env, jobject instance) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    return p->get_session_id();
  }
  return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_virtable_airplay_AirPlaySession_nciSetMirrorHandler(
    JNIEnv *env, jobject instance, jobject handler) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    if (handler) {
      IAirPlayMirrorHandler *h = IAirPlayMirrorHandler::attach(env, handler);
      p->setMirrorHandler(h);
    } else {
      p->setMirrorHandler(0);
    }
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_virtable_airplay_AirPlaySession_nciSetVideoHandler(
    JNIEnv *env, jobject instance, jobject handler) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    if (handler) {
      IAirPlayVideoHandler *h = IAirPlayVideoHandler::attach(env, handler);
      p->setVideoHandler(h);
    } else {
      p->setVideoHandler(0);
    }
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_virtable_airplay_AirPlaySession_nciDisconnect(JNIEnv *env,
                                                             jobject instance) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    p->disconnect();
  }
};

AirPlaySession::AirPlaySession(JNIEnv *env)
    : nci_object<AirPlaySession, AirPlaySession_cls>() {
  (void)env;
}

void AirPlaySession::set_ap_session(
    const ap_session_ptr &p) {
  session_ = p;
}

uint64_t AirPlaySession::get_session_id() {
  auto p = session_.lock();
  if (p) {
    return p->get_session_id();
  }
  return 0;
}

int AirPlaySession::get_session_type() {
  auto p = session_.lock();
  if (p) {
    return p->get_session_type();
  }
  return -1;
}

void AirPlaySession::setMirrorHandler(IAirPlayMirrorHandler *handler) {
  auto p = session_.lock();
  if (p) {
    if (handler) {
      auto h = handler->get_ap_mirror_session_handler();
      p->set_mirror_handler(h);
    } else {
      p->set_mirror_handler(0);
    }
  }
}

void AirPlaySession::setVideoHandler(IAirPlayVideoHandler *handler) {
  auto p = session_.lock();
  if (p) {
    if (handler) {
      auto h = handler->get_ap_video_session_handler();
      p->set_video_handler(h);
    } else {
      p->set_video_handler(0);
    }
  }
}

void AirPlaySession::disconnect() {
  auto p = session_.lock();
  if (p) {
    p->disconnect();
  }
}
