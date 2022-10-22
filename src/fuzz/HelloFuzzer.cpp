/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#include <stddef.h>
#include <stdint.h>

bool FuzzMe(const uint8_t *Data, size_t DataSize) {
  return DataSize >= 3 && Data[0] == 'F' && Data[1] == 'U' && Data[2] == 'Z' &&
         Data[3] == 'Z';  // :‑<
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzMe(Data, Size);
  return 0;
}
