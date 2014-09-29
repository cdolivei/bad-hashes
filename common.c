#include "common.h"
#include <string.h>
#include <stdio.h>

void print_bin(const void *ptr, uint32 len)
{
	int i = 0, j;
	const char *p = (char*)ptr;

	fprintf(stderr, "Binary dump of %p is : \n", ptr);
	for (; i < len; ++i) {
		char b[9] = {0};
		for (j = 128; j > 0; j >>= 1) {
			strcat(b, ((p[i] & j) == j) ? "1" : "0");
		}
		fprintf(stderr, "%s ", b);
	}
	fprintf(stderr, "\n");
}

void print_hex(const void *ptr, uint32 size)
{
	int i = 0;
	const char *p = ptr;
	print("Hex dump of %p is : \n", ptr);
	for (; i < size; ++p, ++i) \
		fprintf(stderr, "%hhX ", *p);
	fprintf(stderr, "\n");
}

uint32 endian32(uint32 in)
{
	return ((in & 0xff) << 24) | ((in & 0xff00) << 8) | ((in >> 8) & 0xff00) | ((in >> 24) & 0xff);
}

uint64 endian64(uint64 in)
{
	return ((in & 0xff) << 56) | ((in & 0xff00) << 40) | ((in & 0xff0000) << 24) | ((in & 0xff000000) << 8) |
	       ((in >> 8) & 0xff000000) | ((in >> 24) & 0xff0000) | ((in >> 40) & 0xff00) | ((in >> 56) & 0xff);
}


