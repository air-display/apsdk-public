// clang-format off
#include <dlfcn.h>
#include <string>
#include "AirPlaySession.h"
// clang-format on

DEFINE_NCI_METHODS(AirPlaySession);

extern "C" JNIEXPORT jint JNICALL
Java_com_sheensoftlab_apsdk_AirPlaySession_nciGetSessionType(
    JNIEnv *env, jobject instance) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    return p->get_session_type();
  }
  return 0;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_sheensoftlab_apsdk_AirPlaySession_nciGetSessionId(
    JNIEnv *env, jobject instance) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    return p->get_session_id();
  }
  return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_sheensoftlab_apsdk_AirPlaySession_nciSetMirrorHandler(
    JNIEnv *env, jobject instance, jobject handler) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    if (handler) {
      IAirPlayMirroringHandler *h = IAirPlayMirroringHandler::attach(env, handler);
      p->setMirroringHandler(h);
    } else {
      p->setMirroringHandler(0);
    }
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_sheensoftlab_apsdk_AirPlaySession_nciSetVideoHandler(
    JNIEnv *env, jobject instance, jobject handler) {
  AirPlaySession *p = AirPlaySession::get(env, instance);
  if (p) {
    if (handler) {
      IAirPlayCastingHandler *h = IAirPlayCastingHandler::attach(env, handler);
      p->setCastingHandler(h);
    } else {
      p->setCastingHandler(0);
    }
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_sheensoftlab_apsdk_AirPlaySession_nciDisconnect(JNIEnv *env,
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

void AirPlaySession::setMirroringHandler(IAirPlayMirroringHandler *handler) {
  auto p = session_.lock();
  if (p) {
    if (handler) {
      auto h = handler->get_mirroring_session_handler();
      p->set_mirroring_handler(h);
    } else {
      p->set_mirroring_handler(0);
    }
  }
}

void AirPlaySession::setCastingHandler(IAirPlayCastingHandler *handler) {
  auto p = session_.lock();
  if (p) {
    if (handler) {
      auto h = handler->get_casting_session_handler();
      p->set_casting_handler(h);
    } else {
      p->set_casting_handler(0);
    }
  }
}

void AirPlaySession::disconnect() {
  auto p = session_.lock();
  if (p) {
    p->disconnect();
  }
}
