//
// Created by shiontian on 11/11/2018.
//

#ifndef APS_SDK_AIRPLAY_SERVER_H
#define APS_SDK_AIRPLAY_SERVER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
#include "jni_class_wrapper.h"
#include "IAirPlayHandler.h"
// clang-format on

using namespace aps;

static const char AirPlayServer_cls[] = "com/virtable/airplay/AirPlayServer";
class AirPlayServer : public nci_object<AirPlayServer, AirPlayServer_cls> {
public:
  AirPlayServer(JNIEnv *env);

  bool start();

  void stop();

  void setConfig(const AirPlayConfig &config);

  void setHandler(IAirPlayHandler *handler);

  uint16_t getServicePort();

private:
  ap_server_ptr server_;
};

#endif // APS_SDK_AIRPLAY_SERVER_H
