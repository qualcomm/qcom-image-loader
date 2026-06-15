// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#ifndef __DEVICEPROGRAMMER_SHA_H__
#define __DEVICEPROGRAMMER_SHA_H__
#include <stdint.h>
#define CONTEXT_LEFTOVER_FIELD_SIZE 64

struct __sechsh_ctx_s
{
   uint32_t counter[2];
   uint32_t iv[16]; // is 64 byte for SHA2-512
   uint8_t leftover[CONTEXT_LEFTOVER_FIELD_SIZE];
   uint32_t leftover_size;
};

void sechsharm_sha256_init(struct __sechsh_ctx_s*);
void sechsharm_sha256_update(struct __sechsh_ctx_s*, uint8_t*, uint32_t*, uint8_t*, uint32_t);
void sechsharm_sha256_final(struct __sechsh_ctx_s*, uint8_t*, uint32_t*, uint8_t*);

#endif /* __DEVICEPROGRAMMER_SHA_H__ */
