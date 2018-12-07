//
// Created by shiontian on 11/11/2018.
//
#include "airplay_server.h"

airplay_server::airplay_server(JNIEnv *env) : nci_object<airplay_server>() {
  server_ = std::make_shared<ap_server>();
}

bool airplay_server::start() {
  if (server_)
    return server_->start();
  return false;
}

void airplay_server::stop() {
  if (server_)
    server_->stop();
}

void airplay_server::setConfig(ap_config_ptr config) {
  if (server_) {
    server_->set_config(config);
  }
}

void airplay_server::setHandler(airplay_handler *handler) {
    if (server_) {
        ap_handler_ptr h = handler->get_ap_handler();
        server_->set_handler(h);
    }
}

short airplay_server::getServicePort() {
  if (server_) {
    return server_->get_service_port();
  }
  return -1;
}

jlong Java_com_medialab_airplay_AirPlayServer_nciNew(JNIEnv *env,
                                                     jobject thiz) {
  airplay_server *p = airplay_server::create(env, thiz);
  return reinterpret_cast<jlong>(p);
}

void Java_com_medialab_airplay_AirPlayServer_nciDelete(JNIEnv *env,
                                                       jobject thiz) {
  airplay_server::destroy(env, thiz);
}

jboolean Java_com_medialab_airplay_AirPlayServer_nciStart(JNIEnv *env,
                                                          jobject thiz) {
  return static_cast<jboolean>(airplay_server::get(env, thiz)->start());
}

void Java_com_medialab_airplay_AirPlayServer_nciStop(JNIEnv *env,
                                                     jobject thiz) {
  airplay_server::get(env, thiz)->stop();
}

void Java_com_medialab_airplay_AirPlayServer_nciSetConfig(JNIEnv *env,
                                                          jobject thiz,
                                                          jobject config) {
  AirPlayConfig airPlayConfig = AirPlayConfig::attach(env, config);

  ap_config_ptr config_ = ap_config::default_instance();
  const char *v = 0;

  jstring name = (jstring)airPlayConfig.name();
  v = env->GetStringUTFChars(name, 0);
  config_->name(v);
  env->ReleaseStringUTFChars(name, v);

  jstring macAddress = (jstring)airPlayConfig.macAddress();
  v = env->GetStringUTFChars(macAddress, 0);
  config_->macAddress(v);
  env->ReleaseStringUTFChars(macAddress, v);

  airplay_server::get(env, thiz)->setConfig(config_);
}

void Java_com_medialab_airplay_AirPlayServer_nciSetHandler(JNIEnv *env,
                                                           jobject thiz,
                                                           jobject handler) {
  auto nci_airplay_ahdnler_ = airplay_handler::get(env, handler);
  if (nci_airplay_ahdnler_) {
    airplay_server::get(env, thiz)->setHandler(nci_airplay_ahdnler_);
  }
}

jshort Java_com_medialab_airplay_AirPlayServer_nciGetServicePort(JNIEnv *env,
                                                           jobject thiz) {
    return airplay_server::get(env, thiz)->getServicePort();
}