//
// Created by shiontian on 11/11/2018.
//
// clang-format off
#include "AirPlayServer.h"
// clang-format on

DEFINE_NCI_METHODS(AirPlayServer);

extern "C" JNIEXPORT jboolean JNICALL
Java_com_virtable_airplay_AirPlayServer_nciStart(JNIEnv *env, jobject thiz) {
  return static_cast<jboolean>(AirPlayServer::get(env, thiz)->start());
}

extern "C" JNIEXPORT void JNICALL
Java_com_virtable_airplay_AirPlayServer_nciStop(JNIEnv *env, jobject thiz) {
  AirPlayServer::get(env, thiz)->stop();
}

extern "C" JNIEXPORT void JNICALL
Java_com_virtable_airplay_AirPlayServer_nciSetConfig(JNIEnv *env, jobject thiz,
                                                     jobject config) {
  env->DeleteLocalRef(config);
  AirPlayConfig airPlayConfig = AirPlayConfig::attach(env, config);
  AirPlayServer::get(env, thiz)->setConfig(airPlayConfig);
}

extern "C" JNIEXPORT void JNICALL
Java_com_virtable_airplay_AirPlayServer_nciSetHandler(JNIEnv *env, jobject thiz,
                                                      jobject handler) {
  if (handler) {
    auto airplayHandler = IAirPlayHandler::attach(env, handler);
    if (airplayHandler) {
      AirPlayServer::get(env, thiz)->setHandler(airplayHandler);
    }
  } else {
    AirPlayServer::get(env, thiz)->setHandler(0);
  }
}

extern "C" JNIEXPORT jint JNICALL
Java_com_virtable_airplay_AirPlayServer_nciGetServicePort(JNIEnv *env,
                                                          jobject thiz) {
  return AirPlayServer::get(env, thiz)->getServicePort();
}

AirPlayServer::AirPlayServer(JNIEnv *env)
    : nci_object<AirPlayServer, AirPlayServer_cls>() {
  (void)env;
  server_ = std::make_shared<ap_server>();
}

bool AirPlayServer::start() {
  if (server_)
    return server_->start();
  return false;
}

void AirPlayServer::stop() {
  if (server_)
    server_->stop();
}

void AirPlayServer::setConfig(const AirPlayConfig &config) {
  if (server_) {
    JNIEnv *env = getJNIEnv();
    if (env) {
      ap_config_ptr cfg = ap_config::default_instance();
      const char *s = 0;

      // Get the server name
      jstring name = (jstring) config.name();
      s = env->GetStringUTFChars(name, 0);
      cfg->name(s);
      env->ReleaseStringUTFChars(name, s);

      // Get publish service flag
      jboolean publishService = (jboolean) config.publishService();
      cfg->publishService(publishService);

      // Get the mac address
      jstring macAddress = (jstring) config.macAddress();
      s = env->GetStringUTFChars(macAddress, 0);
      cfg->macAddress(s);
      env->ReleaseStringUTFChars(macAddress, s);
      server_->set_config(cfg);
    }
  }
}

void AirPlayServer::setHandler(IAirPlayHandler *handler) {
  if (server_) {
    if (handler) {
      ap_handler_ptr h = handler->get_ap_handler();
      server_->set_handler(h);
    } else {
      ap_handler_ptr h = 0;
      server_->set_handler(h);
    }
  }
}

uint16_t AirPlayServer::getServicePort() {
  if (server_) {
    return server_->get_service_port();
  }
  return static_cast<uint16_t >(-1);
}