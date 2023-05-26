#ifndef FAIRPLAY_H
#define FAIRPLAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Generates data for fairplay setup phase.
/// </summary>
/// <param name="mode">The mode</param>
/// <param name="out">The buffer to receive the generated data.</param>
void fairplay_setup(const uint8_t mode, uint8_t *out);

/// <summary>
/// Generates data for fairplay handshake phase.
/// </summary>
/// <param name="key_msg">The key message</param>
/// <param name="out">The buffer to receive the generated data.</param>
void fairplay_handshake(const uint8_t *key_msg, uint8_t *out);

/// <summary>
/// Decrypts the fairplay message
/// </summary>
/// <param name="message">The encrypted message</param>
/// <param name="cipher_text">The cipher text</param>
/// <param name="out">The buffer to receive the generated data.</param>
void fairplay_decrypt(uint8_t *message, uint8_t *cipher_text, uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif
