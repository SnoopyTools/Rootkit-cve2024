#include <IOKit/IOKitLib.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>
#include <time.h>

// int maybe() {
//   static int seeded = 0;
//   if (!seeded) {
//     srand(time(NULL));
//     seeded = 1;
//   }
//   return !(rand() % 100);
// }

// void flip_bit(void *buf, size_t len) {
//   if (!len)
//     return;
//   size_t offset = rand() % len;
//   ((uint8_t *)buf)[offset] ^= (0x01 << (rand() % 8));
// }


void flip_bit(void *buf, size_t len) {

  if (!len)
    return;
  size_t offset = rand() % len;

  ((int *)buf)[offset] = 0x41414141;
}

kern_return_t fake_IOConnectCallMethod(mach_port_t connection,
                                       uint32_t selector, uint64_t *input,
                                       uint32_t inputCnt, void *inputStruct,
                                       size_t inputStructCnt, uint64_t *output,
                                       uint32_t *outputCnt, void *outputStruct,
                                       size_t *outputStructCntP) {

  flip_bit(inputStruct, inputStructCnt);

  return IOConnectCallMethod(connection, selector, input, inputCnt, inputStruct,
                             inputStructCnt, output, outputCnt, outputStruct,
                             outputStructCntP);
}

typedef struct interposer {
  void *replacement;
  void *original;
} interpose_t;

__attribute__((used)) static const interpose_t interposers[]
    __attribute__((section("__DATA, __interpose"))) = 
    {
        {
          .replacement = (void *)fake_IOConnectCallMethod,
          .original = (void *)IOConnectCallMethod
        }
    };





// void get_arg() {
//   void *handle = dlopen(NULL, RTLD_LAZY);
//   arg = (char **)dlsym(handle, "arg1");
//   printf("arg1 from the main arg: %s\n", *arg);
//   dlclose(handle);
// }

// void printStackTrace() {
//   void *callstack[128];
//   int frames = backtrace(callstack, sizeof(callstack) /
//   sizeof(callstack[0]));

//   char **symbols = backtrace_symbols(callstack, frames);
//   if (symbols == NULL) {
//     perror("backtrace_symbols");
//     exit(EXIT_FAILURE);
//   }

//   for (int i = 0; i < frames; i++) {
//     printf("%s\n", symbols[i]);
//   }

//   free(symbols);
// }

// char **arg;