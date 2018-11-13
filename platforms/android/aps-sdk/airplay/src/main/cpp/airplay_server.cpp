//
// Created by shiontian on 11/11/2018.
//
#include "airplay_server.h"

airplay_server::airplay_server(jobject o)
    : nci_object<airplay_server>(), thiz_(o) {
  server_ = std::make_shared<ap_server>();
}

void airplay_server::initialize(JavaVM *vm, JNIEnv *env) {}

bool airplay_server::start() {
  if (server_)
    return server_->start();
  return false;
}

void airplay_server::stop() {
  if (server_)
    server_->stop();
}

void airplay_server::setConfig() {
  ap_config_ptr config = ap_config::default_instance();
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

jlong Java_com_medialab_airplay_AirPlayServer_nciNew(JNIEnv *env,
                                                     jobject thiz) {
  return (jlong)airplay_server::create(env, thiz);
}

void Java_com_medialab_airplay_AirPlayServer_nciDelete(JNIEnv *env,
                                                       jobject thiz) {
  airplay_server::destroy(env, thiz);
}

jboolean Java_com_medialab_airplay_AirPlayServer_nciStart(JNIEnv *env,
                                                          jobject thiz) {
  return airplay_server::get(env, thiz)->start();
}

void Java_com_medialab_airplay_AirPlayServer_nciStop(JNIEnv *env,
                                                     jobject thiz) {
  airplay_server::get(env, thiz)->stop();
}

void Java_com_medialab_airplay_AirPlayServer_nciSetConfig(JNIEnv *env,
                                                          jobject thiz) {
  airplay_server::get(env, thiz)->setConfig();
}

void Java_com_medialab_airplay_AirPlayServer_nciSetHandler(JNIEnv *env,
                                                           jobject thiz,
                                                           jobject handler) {
  auto nci_airplay_ahdnler_ = airplay_handler::get(env, handler);
  if (nci_airplay_ahdnler_) {
    airplay_server::get(env, thiz)->setHandler(nci_airplay_ahdnler_);
  }
}