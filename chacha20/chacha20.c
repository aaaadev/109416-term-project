// SPDX-License-Identifier: MIT

#include "chacha20.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

static atomic_uint_fast8_t dummy[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void chacha20_init(struct ChaCha20Ctx *ctx, const uint8_t *key,
                   const uint8_t *nonce) {
  memcpy(ctx->state, SALSA20, 16);
  memcpy(ctx->state + 4, key, 32);
  ctx->state[12] = 0;
  ctx->state[13] = 0;
  memcpy(ctx->state + 14, nonce, 12);
  ctx->available = 0;
  return;
}

void chacha20_block(struct ChaCha20Ctx *ctx, uint8_t *output) {
  memcpy(ctx->buffer, ctx->state, sizeof(ctx->buffer));
  for (int i = 0; i < 10; ++i) {
    qr(&ctx->buffer[0], &ctx->buffer[4], &ctx->buffer[8], &ctx->buffer[12]);
    qr(&ctx->buffer[1], &ctx->buffer[5], &ctx->buffer[9], &ctx->buffer[13]);
    qr(&ctx->buffer[2], &ctx->buffer[6], &ctx->buffer[10], &ctx->buffer[14]);
    qr(&ctx->buffer[3], &ctx->buffer[7], &ctx->buffer[11], &ctx->buffer[15]);
    qr(&ctx->buffer[0], &ctx->buffer[5], &ctx->buffer[10], &ctx->buffer[15]);
    qr(&ctx->buffer[1], &ctx->buffer[6], &ctx->buffer[11], &ctx->buffer[12]);
    qr(&ctx->buffer[2], &ctx->buffer[7], &ctx->buffer[8], &ctx->buffer[13]);
    qr(&ctx->buffer[3], &ctx->buffer[4], &ctx->buffer[9], &ctx->buffer[14]);
  }
  for (int i = 0; i < 16; ++i) {
    ctx->buffer[i] += ctx->state[i];
  }
  for (int i = 0; i < 16; ++i) {
    output[4 * i + 0] = ctx->buffer[i] & 0xff;
    output[4 * i + 1] = (ctx->buffer[i] >> 8) & 0xff;
    output[4 * i + 2] = (ctx->buffer[i] >> 16) & 0xff;
    output[4 * i + 3] = (ctx->buffer[i] >> 24) & 0xff;
  }
  if (++ctx->state[12] == 0) {
    ++ctx->state[13];
  }
  return;
}

void chacha20_xor(struct ChaCha20Ctx *ctx, const uint8_t *input,
                  uint8_t *output, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    if (ctx->available == 0) {
      chacha20_block(ctx, ctx->state);
      ctx->available = sizeof(ctx->state);
    }
    output[i] = input[i] ^ ctx->state[sizeof(ctx->state) - ctx->available--];
  }
  return;
}

uint8_t *chacha20_random(size_t size, const uint8_t *key,
                         const uint8_t *nonce) {
  uint8_t *buffer = malloc(sizeof(uint8_t) * size);
  struct ChaCha20Ctx ctx;
  chacha20_init(&ctx, key, nonce);
  chacha20_xor(&ctx, buffer, buffer, size);
  return buffer;
}

uint64_t chacha20_random64() {
  const size_t size = 8;
  uint8_t key[KEY_LEN] = {};
  uint8_t nonce[NONCE_LEN] = {};
  for (size_t i = 0; i < KEY_LEN - size; i++)
    key[i] = (uint8_t)(rand() % (1 << 8));
  for (size_t i = KEY_LEN - size; i < KEY_LEN; i++)
    key[i] = atomic_load(&dummy[i - (KEY_LEN - size)]);
  for (size_t i = 0; i < NONCE_LEN - size; i++)
    nonce[i] = (uint8_t)(rand() % (1 << 8));
  for (size_t i = NONCE_LEN - size; i < NONCE_LEN; i++)
    nonce[i] = atomic_load(&dummy[i - (NONCE_LEN - size)]);
  uint8_t *buffer = chacha20_random(size, key, nonce);
  uint64_t res = 0;
  for (size_t i = 0; i < size; i++) {
    atomic_store(&dummy[i], buffer[i]);
    res += buffer[i];
    res = res << 8;
  }
  return res;
}
