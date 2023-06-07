// SPDX-License-Identifier: MIT

#ifndef CHACHA20_H
#define CHACHA20_H

#define ROUNDS 20
#define KEY_LEN 32
#define NONCE_LEN 12

#include <stdint.h>
#include <stdlib.h>

typedef struct ChaCha20Ctx {
  uint32_t state[16];
  uint32_t buffer[16];
  size_t available;
};

// https://en.wikipedia.org/wiki/Salsa20#ChaCha_variant from pseudo C code
static const char SALSA20[17] = "expand 32-byte k";

static uint32_t rotl(uint32_t a, int b) { return (a << b) | (a >> (32 - b)); }

static void qr(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
  *a += *b;
  *d = rotl(*d ^ *a, 16);
  *c += *d;
  *b = rotl(*b ^ *c, 12);
  *a += *b;
  *d = rotl(*d ^ *a, 8);
  *c += *d;
  *b = rotl(*b ^ *c, 7);
}

// PUBLIC fields

extern void chacha20_init(struct ChaCha20Ctx *ctx, const uint8_t *key,
                          const uint8_t *nonce);
extern void chacha20_block(struct ChaCha20Ctx *ctx, uint8_t *output);
extern void chacha20_xor(struct ChaCha20Ctx *ctx, const uint8_t *input,
                         uint8_t *output, size_t size);
extern uint8_t *chacha20_random(size_t size, const uint8_t *key,
                                const uint8_t *nonce);
extern uint64_t chacha20_random64();

#endif