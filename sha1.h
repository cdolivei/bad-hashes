#if !defined(__SHA1__)
#define __SHA1__

#include "common.h"

#define BLOCK_SIZE 64
#define MAX_SIZE 8

inline void initializeHashValue(uint32 H[5]);

int padding(const char*, uint64, uint32, uint32, char**, uint32*);
int sha1_hash_computation(const char*, uint32[5], void callback(uint32, uint32, uint32, uint32, uint32, uint32));

#endif
