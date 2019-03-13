#ifndef _AP_AES_CTR128_H_
#define _AP_AES_CTR128_H_
#pragma once

#include <array>

#include <opensslaes/aes.h>

class ap_aes_ctr128 {
public:
  ap_aes_ctr128();
  ~ap_aes_ctr128();

  void set_key_iv(uint8_t *key, uint8_t *iv);

  void xcrypt_buffer(uint8_t *buffer, uint32_t length);

private:
  void reset();

private:
  AES_KEY ctx_;
  uint32_t number;
  std::array<uint8_t, 16> iv_;
  std::array<uint8_t, 16> ec_;
};

class ap_aes_cbc128 {
public:
  ap_aes_cbc128();
  ~ap_aes_cbc128();

  void set_en_key_iv(uint8_t *key, uint8_t *iv);

  void set_de_key_iv(uint8_t *key, uint8_t *iv);

  void encrypt_buffer(uint8_t *buffer, uint32_t length);

  void decrypt_buffer(uint8_t *buffer, uint32_t length);

private:
  void reset();

private:
  AES_KEY en_ctx_;
  AES_KEY de_ctx_;
  std::array<uint8_t, 16> en_iv_;
  std::array<uint8_t, 16> de_iv_;
};

#endif // _AP_AES_CTR128_H_
