/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

bool FuzzMe(const uint8_t *Data, size_t DataSize) {
  return DataSize >= 3 && Data[0] == 'F' && Data[1] == 'U' && Data[2] == 'Z' &&
         Data[3] == 'Z';  // :‑<
}

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv) {
  printf("I'm initialized, %d, %p...\n", *argc, argv);
  return 0;
}

int foo() {
  printf("I'm called\n");
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  //   static int ret = foo();
  //   if (ret != 0) {
  //     printf("I'm fucked!\n");
  //   } else {
  //     // printf("I'm ok\n");
  //   }

  FuzzMe(Data, Size);
  return 0;
}
