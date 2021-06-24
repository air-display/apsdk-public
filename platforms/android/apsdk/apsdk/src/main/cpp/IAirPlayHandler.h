//
// Created by shiontian on 11/12/2018.
//

#ifndef APS_SDK_AIRPLAY_HANDLER_H
#define APS_SDK_AIRPLAY_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
// clang-format on

using namespace aps;

static const char IAirPlayHandler_cls[] =
    "com/virtable/airplay/IAirPlayHandler";
class IAirPlayHandler
    : public jni_meta_object<IAirPlayHandler, IAirPlayHandler_cls> {

public:
  IAirPlayHandler(JNIEnv *env);

  ap_handler_ptr get_ap_handler();

  void on_session_begin(ap_session_ptr session);

  void on_session_end(const uint64_t session_id);

private:
  jni_ap_handler_ptr handler_;
};

#endif // APS_SDK_AIRPLAY_HANDLER_H