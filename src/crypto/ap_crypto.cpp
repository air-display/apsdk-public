#include <array>
#include <sstream>

#include <asio.hpp>
#include <curve25519/curve25519-donna.h>
#include <ed25519/ed25519.h>
#include <ed25519/sha512.h>
#include <playfair/playfair.h>
#include <utils/utils.h>

#include "ap_crypto.h"

aps::server_key_chain::server_key_chain() {
  curve_private_key_.resize(32, 0);
  ed25519_create_seed(curve_private_key_.data());

  curve_public_key_.resize(32, 0);
  curve25519_donna(curve_public_key_.data(), curve_private_key_.data(), 0);

  std::array<uint8_t, 32> seed;
  ed_private_key_.resize(64, 0);
  ed_public_key_.resize(32, 0);
  ed25519_create_seed(seed.data());
  ed25519_create_keypair(ed_public_key_.data(), ed_private_key_.data(),
                         seed.data());
}

aps::server_key_chain::~server_key_chain() {}

const std::vector<uint8_t> &aps::server_key_chain::ed_public_key() const {
  return ed_public_key_;
}

const std::vector<uint8_t> &aps::server_key_chain::ed_private_key() const {
  return ed_private_key_;
}

const std::vector<uint8_t> &aps::server_key_chain::curve_public_key() const {
  return curve_public_key_;
}

const std::vector<uint8_t> &aps::server_key_chain::curve_private_key() const {
  return curve_private_key_;
}

aps::ap_crypto::ap_crypto()
    : pair_verifyed_(false), fp_key_message_(164), client_aes_key_(16),
      client_aes_iv_(16), client_curve_public_key_(32),
      client_ed_public_key_(32), shared_secret_(32) {}

aps::ap_crypto::~ap_crypto() {}

void aps::ap_crypto::init_client_public_keys(const uint8_t *pcurve,
                                             uint64_t curve_len,
                                             const uint8_t *ped,
                                             uint64_t ed_len) {
  if (pcurve && curve_len) {
    this->client_curve_public_key_.assign(pcurve, pcurve + curve_len);
  }

  if (ped && ed_len) {
    this->client_ed_public_key_.assign(ped, ped + ed_len);
  }
}

void aps::ap_crypto::init_pair_verify_aes() {
  // Fixed AES key and IV
  static const char *AES_KEY = "Pair-Verify-AES-Key";
  static const char *AES_IV = "Pair-Verify-AES-IV";

  shared_secret_.resize(32, 0);
  curve25519_donna(shared_secret_.data(), server_.curve_private_key().data(),
                   client_curve_public_key_.data());

  sha512_context_ sha512_context;
  std::array<uint8_t, 64> sha512_hash;

  sha512_hash.fill(0);
  sha512_init(&sha512_context);
  sha512_update(&sha512_context, (uint8_t *)AES_KEY, std::strlen(AES_KEY));
  sha512_update(&sha512_context, shared_secret_.data(), 32);
  sha512_final(&sha512_context, sha512_hash.data());

  std::array<uint8_t, 16> aes_key;
  memcpy(aes_key.data(), sha512_hash.data(), 16);

  sha512_hash.fill(0);
  sha512_init(&sha512_context);
  sha512_update(&sha512_context, (uint8_t *)AES_IV, std::strlen(AES_IV));
  sha512_update(&sha512_context, shared_secret_.data(), 32);
  sha512_final(&sha512_context, sha512_hash.data());

  std::array<uint8_t, 16> aes_iv;
  memcpy(aes_iv.data(), sha512_hash.data(), 16);

  pair_verify_aes_ctr_.set_key_iv(aes_key.data(), aes_iv.data());

  pair_verifyed_ = true;
}

void aps::ap_crypto::sign_pair_signature(std::vector<uint8_t> &sig) {
  std::array<uint8_t, 64> message;
  memcpy(message.data(), server_.curve_public_key().data(), 32);
  memcpy(message.data() + 32, client_curve_public_key_.data(), 32);

  std::array<uint8_t, 64> signature;
  ed25519_sign(signature.data(), message.data(), message.size(),
               server_.ed_public_key().data(), server_.ed_private_key().data());

  pair_verify_aes_ctr_.xcrypt_buffer(signature.data(), signature.size());

  sig.assign(signature.begin(), signature.end());
}

bool aps::ap_crypto::verify_pair_signature(const uint8_t *p, uint64_t len) {
  std::vector<uint8_t> encrypted_signature;
  encrypted_signature.assign(p, p + len);

  std::vector<uint8_t> plain_signature = encrypted_signature;
  pair_verify_aes_ctr_.xcrypt_buffer(plain_signature.data(),
                                     plain_signature.size());

  std::vector<uint8_t> message = client_curve_public_key_;
  std::copy(server_.curve_public_key().begin(),
            server_.curve_public_key().end(), std::back_inserter(message));

  return ed25519_verify(plain_signature.data(), message.data(), message.size(),
                        client_ed_public_key_.data());
}

void aps::ap_crypto::fp_setup(const uint8_t mode, uint8_t *content) {
  uint8_t *pos = (uint8_t *)&(reply_message[mode]);
  memcpy(content, pos, 142);
}

void aps::ap_crypto::fp_handshake(uint8_t *content, const uint8_t *keymsg,
                                  const uint32_t len) {
  if (!keymsg)
    return;

  this->fp_key_message_.assign(keymsg, keymsg + len);

  uint8_t *pos = (uint8_t *)&(fp_header);
  memcpy(content, pos, 12);
  memcpy(content + 12, &keymsg[144], 20);
}

void aps::ap_crypto::fp_decrypt(const uint8_t *key, uint8_t *out) {
  playfair_decrypt(fp_key_message_.data(), (uint8_t *)key, out);
}

void aps::ap_crypto::init_client_aes_info(const uint8_t *piv, uint64_t iv_len,
                                          const uint8_t *pkey,
                                          uint64_t key_len) {
  if (piv && iv_len) {
    this->client_aes_iv_.assign(piv, piv + iv_len);
  }

  if (pkey && key_len) {
    fp_decrypt(pkey, client_aes_key_.data());

    if (pair_verifyed_) {
      sha512_context_ sha512_context;
      std::array<uint8_t, 64> sha512_hash;
      sha512_hash.fill(0);
      sha512_init(&sha512_context);
      sha512_update(&sha512_context, (uint8_t *)client_aes_key_.data(), 16);
      sha512_update(&sha512_context, shared_secret_.data(), 32);
      sha512_final(&sha512_context, sha512_hash.data());
      std::copy(sha512_hash.begin(), sha512_hash.begin() + 16,
                std::back_inserter(client_aes_key_));
    }
  }
}

void aps::ap_crypto::init_video_stream_aes_ctr(const uint64_t video_stream_id,
                                               const agent_version_t version) {
  sha512_context_ sha512_context;
  std::array<uint8_t, 64> sha512_hash;

  std::ostringstream oss;
  std::string source;
  oss.str("");
  oss << "AirPlayStreamKey" << video_stream_id;
  source = oss.str();
  sha512_hash.fill(0);
  sha512_init(&sha512_context);
  sha512_update(&sha512_context, (uint8_t *)source.c_str(), source.length());
  sha512_update(&sha512_context, client_aes_key_.data(),
                client_aes_key_.size());
  sha512_final(&sha512_context, sha512_hash.data());
  std::array<uint8_t, 16> aes_key;
  memcpy(aes_key.data(), sha512_hash.data(), 16);

  oss.str("");
  oss << "AirPlayStreamIV" << video_stream_id;
  source = oss.str();
  sha512_hash.fill(0);
  sha512_init(&sha512_context);
  sha512_update(&sha512_context, (uint8_t *)source.c_str(), source.length());
  sha512_update(&sha512_context, client_aes_key_.data(),
                client_aes_key_.size());
  sha512_final(&sha512_context, sha512_hash.data());
  std::array<uint8_t, 16> aes_iv;
  memcpy(aes_iv.data(), sha512_hash.data(), 16);

  mirror_stream_aes_ctr_.set_key_iv(aes_key.data(), aes_iv.data());
}

void aps::ap_crypto::init_audio_stream_aes_cbc() {}

void aps::ap_crypto::decrypt_video_frame(uint8_t *frame, uint32_t len) {
  mirror_stream_aes_ctr_.xcrypt_buffer(frame, len);
}

void aps::ap_crypto::decrypt_audio_data(uint8_t *data, uint32_t len) {
  audio_stream_aes_cbc_.set_de_key_iv(client_aes_key_.data(),
                                      client_aes_iv_.data());
  audio_stream_aes_cbc_.decrypt_buffer(data, len);
}

const std::vector<uint8_t> &aps::ap_crypto::fp_key_message() const {
  return fp_key_message_;
}

const std::vector<uint8_t> &aps::ap_crypto::shared_secret() const {
  return shared_secret_;
}

const std::vector<uint8_t> &aps::ap_crypto::client_aes_key() const {
  return client_aes_key_;
}

const std::vector<uint8_t> &aps::ap_crypto::client_aes_iv() const {
  return client_aes_iv_;
}

const std::vector<uint8_t> &aps::ap_crypto::client_ed_public_key() const {
  return client_ed_public_key_;
}

const std::vector<uint8_t> &aps::ap_crypto::client_curve_public_key() const {
  return client_curve_public_key_;
}

const aps::server_key_chain &aps::ap_crypto::server_keys() const {
  return server_;
}
