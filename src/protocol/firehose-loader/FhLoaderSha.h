// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:
//source/qcom/qct/core/storage/tools/fh_loader/vs2013/fh_loader/fh_loader_sha.h#1
$ $DateTime: 2016/01/15 16:34:27 $ $Author: wkimberl $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-01-14   wek   Create. Move SHA functions from security to a new file.

===========================================================================*/


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
