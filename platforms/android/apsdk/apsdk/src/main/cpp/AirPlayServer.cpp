/*
 *  File: AirPlayServer.cpp
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public)
 *  Copyright (C) 2018-2024 Sheen Tian
 *
 *  apsdk is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Foobar.
 *  If not, see <https://www.gnu.org/licenses/>.
 */

// clang-format off
#include "AirPlayServer.h"
// clang-format on

DEFINE_NCI_METHODS(AirPlayServer);

extern "C" JNIEXPORT jboolean JNICALL Java_com_sheentech_apsdk_AirPlayServer_nciStart(JNIEnv *env, jobject thiz) {
  return static_cast<jboolean>(AirPlayServer::get(env, thiz)->start());
}

extern "C" JNIEXPORT void JNICALL Java_com_sheentech_apsdk_AirPlayServer_nciStop(JNIEnv *env, jobject thiz) {
  AirPlayServer::get(env, thiz)->stop();
}

extern "C" JNIEXPORT void JNICALL Java_com_sheentech_apsdk_AirPlayServer_nciSetConfig(JNIEnv *env, jobject thiz,
                                                                                      jobject config) {
  env->DeleteLocalRef(config);
  AirPlayConfig airPlayConfig = AirPlayConfig::attach(env, config);
  AirPlayServer::get(env, thiz)->setConfig(airPlayConfig);
}

extern "C" JNIEXPORT void JNICALL Java_com_sheentech_apsdk_AirPlayServer_nciSetHandler(JNIEnv *env, jobject thiz,
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

extern "C" JNIEXPORT jint JNICALL Java_com_sheentech_apsdk_AirPlayServer_nciGetServicePort(JNIEnv *env, jobject thiz) {
  return AirPlayServer::get(env, thiz)->getServicePort();
}

AirPlayServer::AirPlayServer(JNIEnv *env) : nci_object<AirPlayServer, AirPlayServer_cls>() {
  (void)env;
  aps::ap_server::setJavaVM(getJavaVM());
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
      jstring name = (jstring)config.name();
      s = env->GetStringUTFChars(name, 0);
      cfg->name(s);
      env->ReleaseStringUTFChars(name, s);

      // Get publish service flag
      jboolean publishService = (jboolean)config.publishService();
      cfg->publishService(publishService);

      // Get the mac address
      jstring macAddress = (jstring)config.macAddress();
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
  return static_cast<uint16_t>(-1);
}
