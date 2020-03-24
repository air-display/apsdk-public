//
// Created by shiontian on 11/11/2018.
//

#ifndef APS_SDK_AIRPLAY_SESSION_H
#define APS_SDK_AIRPLAY_SESSION_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "IAirPlayMirrorHandler.h"
#include "IAirPlayVideoHandler.h"
// clang-format on

using namespace aps;

static const char AirPlaySession_cls[] =
    "com/virtable/airplay/AirPlaySession";
class AirPlaySession
    : public nci_object<AirPlaySession, AirPlaySession_cls> {
public:
  AirPlaySession(JNIEnv *env);

  void set_ap_session(const ap_session_ptr &p);

  uint64_t get_session_id();

  int get_session_type();

  void setMirrorHandler(IAirPlayMirrorHandler *handler);

  void setVideoHandler(IAirPlayVideoHandler *handler);

  void disconnect();

private:
  ap_session_weakptr session_;
};

#endif // APS_SDK_AIRPLAY_SESSION_H
