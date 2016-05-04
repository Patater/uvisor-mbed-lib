/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __UVISOR_LIB_MALLOC_H__
#define __UVISOR_LIB_MALLOC_H__

#include "uvisor_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sets the allocator to be used from now on.
 * @retval 0  if allocator valid (non-NULL)
 * @retval -1 if allocator invalid
 */
int uvisor_set_allocator(UvisorAllocator allocator);

/** @returns the currently active allocator */
UvisorAllocator uvisor_get_allocator(void);

/* Allocate memory on the process heap */
void *malloc_p(size_t size);
/* Reallocate memory on the process heap */
void *realloc_p(void *ptr, size_t size);
/* Free memory on the process heap */
void free_p(void *ptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* __UVISOR_LIB_MALLOC_H__ */
