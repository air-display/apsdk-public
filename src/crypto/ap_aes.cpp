#include "ap_aes.h"

ap_aes_ctr128::ap_aes_ctr128() { reset(); }

ap_aes_ctr128::~ap_aes_ctr128() { reset(); }

void ap_aes_ctr128::set_key_iv(uint8_t *key, uint8_t *iv) {
  reset();
  AES_set_encrypt_key(key, 128, &ctx_);
  memcpy(iv_.data(), iv, 16);
}

void ap_aes_ctr128::xcrypt_buffer(uint8_t *buffer, uint32_t length) {
  AES_ctr128_encrypt(buffer, buffer, length, &ctx_, iv_.data(), ec_.data(),
                     &number);
}

void ap_aes_ctr128::reset() {
  memset(&ctx_, 0, sizeof(AES_KEY));
  number = 0;
  iv_.fill(0);
  ec_.fill(0);
}

ap_aes_cbc128::ap_aes_cbc128() {}

ap_aes_cbc128::~ap_aes_cbc128() {}

void ap_aes_cbc128::set_key_iv(uint8_t *key, uint8_t *iv) { reset(); }

void ap_aes_cbc128::encrypt_buffer(uint8_t *buffer, uint32_t length) {}

void ap_aes_cbc128::decrypt_buffer(uint8_t *buffer, uint32_t length) {}

void ap_aes_cbc128::reset() {
  memset(&ctx_, 0, sizeof(AES_KEY));
  number = 0;
  iv_.fill(0);
  ec_.fill(0);
}
