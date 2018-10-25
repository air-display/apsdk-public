#include <ed25519/ed25519.h>
#include <ed25519/sha512.h>
#include <curve25519/curve25519-donna.h>
#include "ap_crypto.h"

aps::server_key_chain::server_key_chain()
{
    curve_private_key_.resize(32, 0);
    ed25519_create_seed(
        curve_private_key_.data());

    curve_public_key_.resize(32, 0);
    curve25519_donna(
        curve_public_key_.data(),
        curve_private_key_.data(), 0);

    uint8_t seed[32];
    ed_private_key_.resize(64, 0);
    ed_public_key_.resize(32, 0);
    ed25519_create_seed(seed);
    ed25519_create_keypair(
        ed_public_key_.data(),
        ed_private_key_.data(), seed);
}

aps::server_key_chain::~server_key_chain()
{
}

const std::vector<uint8_t>& aps::server_key_chain::ed_public_key() const
{
    return ed_public_key_;
}

const std::vector<uint8_t>& aps::server_key_chain::ed_private_key() const
{
    return ed_private_key_;
}

const std::vector<uint8_t>& aps::server_key_chain::curve_public_key() const
{
    return curve_public_key_;
}

const std::vector<uint8_t>& aps::server_key_chain::curve_private_key() const
{
    return curve_private_key_;
}

aps::ap_crypto::ap_crypto()
{
    client_rsa_key_.resize(64);
    client_rsa_iv_.resize(64);
    client_curve_public_key_.resize(32);
    client_ed_public_key_.resize(32);
    shared_secret_.resize(32);
}

aps::ap_crypto::~ap_crypto()
{
}

void aps::ap_crypto::init_client_rsa_info(const uint8_t* piv, uint32_t iv_len, const uint8_t* pkey, uint32_t key_len)
{
    if (piv && iv_len)
    {
        this->client_rsa_iv_.assign(piv, piv + iv_len);
    }

    if (pkey && key_len)
    {
        this->client_rsa_key_.assign(pkey, pkey + key_len);
    }
}

void aps::ap_crypto::init_client_public_keys(const uint8_t* pcurve, uint32_t curve_len, const uint8_t* ped, uint32_t ed_len)
{
    if (pcurve && curve_len)
    {
        this->client_curve_public_key_.assign(
            pcurve, pcurve + curve_len);
    }

    if (ped && ed_len)
    {
        this->client_ed_public_key_.assign(
            ped, ped + ed_len);
    }
}

void aps::ap_crypto::init_pair_verify_aes()
{
    // Fixed AES key and IV
    static const char* AES_KEY = "Pair-Verify-AES-Key";
    static const char* AES_IV = "Pair-Verify-AES-IV";

    shared_secret_.resize(32, 0);
    curve25519_donna(
        shared_secret_.data(),
        server_.curve_private_key().data(),
        client_curve_public_key_.data());

    sha512_context_ sha512_context;
    std::vector<uint8_t> sha512_hash;
    
    sha512_hash.resize(64, 0);
    sha512_init(&sha512_context);
    sha512_update(&sha512_context, (uint8_t*)AES_KEY, std::strlen(AES_KEY));
    sha512_update(&sha512_context, shared_secret_.data(), 32);
    sha512_final(&sha512_context, sha512_hash.data());

    std::vector<uint8_t> sha512_aes_key;
    sha512_aes_key.assign(sha512_hash.begin(), sha512_hash.begin() + 16);

    sha512_hash.resize(64, 0);
    sha512_init(&sha512_context);
    sha512_update(&sha512_context, (uint8_t*)AES_IV, std::strlen(AES_IV));
    sha512_update(&sha512_context, shared_secret_.data(), 32);
    sha512_final(&sha512_context, sha512_hash.data());

    std::vector<uint8_t> sha512_aes_iv;
    sha512_aes_iv.assign(sha512_hash.begin(), sha512_hash.begin() + 16);

    AES_init_ctx_iv(
        &pair_verify_aes_ctr_ctx,
        sha512_aes_key.data(),
        sha512_aes_iv.data());
}

void aps::ap_crypto::sign_pair_signature(std::vector<uint8_t>& sig)
{
    std::vector<uint8_t> message;
    std::copy(server_.curve_public_key().data(),
        server_.curve_public_key().data() + 32,
        std::back_inserter(message));
    std::copy(client_curve_public_key_.data(),
        client_curve_public_key_.data() + 32,
        std::back_inserter(message));

    std::vector<uint8_t> signature(64, 0);
    ed25519_sign(signature.data(),
        message.data(), message.size(),
        server_.ed_public_key().data(),
        server_.ed_private_key().data());

    AES_CTR_xcrypt_buffer(
        &pair_verify_aes_ctr_ctx, 
        signature.data(),
        signature.size());

    sig = signature;
}

bool aps::ap_crypto::verify_pair_signature(const uint8_t* p, uint32_t len)
{
    std::vector<uint8_t> encrypted_signature;
    encrypted_signature.assign(p, p + len);

    std::vector<uint8_t> plain_signature = encrypted_signature;
    AES_CTR_xcrypt_buffer(
        &pair_verify_aes_ctr_ctx, 
        plain_signature.data(),
        plain_signature.size());

    std::vector<uint8_t> message = client_curve_public_key_;
    std::copy(
        server_.curve_public_key().begin(),
        server_.curve_public_key().end(),
        std::back_inserter(message));

    return ed25519_verify(
        plain_signature.data(),
        message.data(), message.size(),
        client_ed_public_key_.data());
}

void aps::ap_crypto::init_video_stream_aes(const uint64_t video_stream_id)
{
    std::string key = "AirPlayStreamKey";
    std::string iv = "AirPlayStreamIV";

    sha512_context_ sha512_context;
    std::vector<uint8_t> sha512_hash;

    sha512_hash.resize(64, 0);
    sha512_init(&sha512_context);
    sha512_update(&sha512_context, (uint8_t*)key.data(), key.length());
    sha512_update(&sha512_context, shared_secret_.data(), 16);
    sha512_final(&sha512_context, sha512_hash.data());

    std::vector<uint8_t> sha512_aes_key;
    sha512_aes_key.assign(sha512_hash.begin(), sha512_hash.begin() + 16);

    sha512_hash.resize(64, 0);
    sha512_init(&sha512_context);
    sha512_update(&sha512_context, (uint8_t*)iv.data(), iv.length());
    sha512_update(&sha512_context, shared_secret_.data(), 16);
    sha512_final(&sha512_context, sha512_hash.data());

    std::vector<uint8_t> sha512_aes_iv;
    sha512_aes_iv.assign(sha512_hash.begin(), sha512_hash.begin() + 16);

    AES_init_ctx_iv(
        &video_stream_aes_ctx,
        sha512_aes_key.data(),
        sha512_aes_iv.data());
}

void aps::ap_crypto::decrypt_video_frame(uint8_t* frame, uint32_t len)
{
    AES_CTR_xcrypt_buffer(&video_stream_aes_ctx, frame, len);
}

const std::vector<uint8_t>& aps::ap_crypto::shared_secret() const
{
    return shared_secret_;
}

const std::vector<uint8_t>& aps::ap_crypto::client_rsa_key() const
{
    return client_rsa_key_;
}

const std::vector<uint8_t>& aps::ap_crypto::client_rsa_iv() const
{
    return client_rsa_iv_;
}

const std::vector<uint8_t>& aps::ap_crypto::client_ed_public_key() const
{
    return client_ed_public_key_;
}

const std::vector<uint8_t>& aps::ap_crypto::client_curve_public_key() const
{
    return client_curve_public_key_;
}

const aps::server_key_chain& aps::ap_crypto::server_keys() const
{
    return server_;
}
