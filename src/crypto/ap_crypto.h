#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <ap_types.h>

#include "ap_aes.h"

namespace aps {
class server_key_chain {
public:
  server_key_chain();
  ~server_key_chain();

  const std::vector<uint8_t> &ed_public_key() const;
  const std::vector<uint8_t> &ed_private_key() const;
  const std::vector<uint8_t> &curve_public_key() const;
  const std::vector<uint8_t> &curve_private_key() const;

private:
  std::vector<uint8_t> ed_public_key_;
  std::vector<uint8_t> ed_private_key_;
  std::vector<uint8_t> curve_public_key_;
  std::vector<uint8_t> curve_private_key_;
};

class ap_crypto {
public:
  ap_crypto();
  ~ap_crypto();

  void init_client_public_keys(const uint8_t *pcurve, uint64_t curve_len,
                               const uint8_t *ped, uint64_t ed_len);

  void init_pair_verify_aes();

  void fp_setup(const uint8_t mode, uint8_t *content);

  void fp_handshake(uint8_t *content, const uint8_t *request,
                    const uint32_t len);

  void fp_decrypt(const uint8_t *key, uint8_t *out);

  void sign_pair_signature(std::vector<uint8_t> &sig);

  bool verify_pair_signature(const uint8_t *p, uint64_t len);

  void init_video_stream_aes_ctr(const uint64_t video_stream_id,
                                 const agent_version_t version);

  void init_client_aes_info(const uint8_t *piv, uint64_t iv_len,
                            const uint8_t *pkey, uint64_t key_len);

  void init_audio_stream_aes_cbc();

  void decrypt_video_frame(uint8_t *frame, uint32_t len);

  void decrypt_audio_data(uint8_t *data, uint32_t len);

  const std::vector<uint8_t> &fp_key_message() const;

  const std::vector<uint8_t> &shared_secret() const;

  const std::vector<uint8_t> &client_aes_key() const;

  const std::vector<uint8_t> &client_aes_iv() const;

  const std::vector<uint8_t> &client_ed_public_key() const;

  const std::vector<uint8_t> &client_curve_public_key() const;

  const server_key_chain &server_keys() const;

private:
  bool pair_verifyed_;
  server_key_chain server_;

  std::vector<uint8_t> fp_key_message_;
  std::vector<uint8_t> shared_secret_;

  std::vector<uint8_t> client_aes_key_;
  std::vector<uint8_t> client_aes_iv_;

  std::vector<uint8_t> client_ed_public_key_;
  std::vector<uint8_t> client_curve_public_key_;

  ap_aes_ctr128 pair_verify_aes_ctr_;
  ap_aes_ctr128 mirror_stream_aes_ctr_;
  ap_aes_cbc128 audio_stream_aes_cbc_;
};

typedef std::shared_ptr<ap_crypto> ap_crypto_ptr;
} // namespace aps
