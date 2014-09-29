#if!defined(__COMMON__)
#define __COMMON__

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#if defined(NDEBUG)
	#define print(format, args...)
	#define print_hex(ptr, size)
	#define print_bin(ptr, size)
#else
	#define print(format, args...) fprintf(stderr, format, ##args)

	void print_bin(const void *, uint32);
	void print_hex(const void *, uint32);

#endif

uint32 endian32(uint32);
uint64 endian64(uint64);

#endif
