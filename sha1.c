#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "sha1.h"
#include "common.h"

#define Ch(x,y,z) ((x&y) ^ (~x&z))
#define Parity(x,y,z) (x^y^z)
#define Maj(x,y,z) ((x&y) | (x&z) | (y&z))

#define BITS(x) (sizeof(x) * 8)

uint32 f(uint32 t, uint32 x, uint32 y, uint32 z)
{
	assert(t < 80);
	if (t < 20)
		return Ch(x,y,z);
	else if (t < 40)
		return Parity(x,y,z);
	else if (t < 60)
		return Maj(x,y,z);
	return Parity(x,y,z);
}

inline uint32 ROTL(uint32 n, uint32 x)
{
	assert(n < 32);
	return ((x << n) | (x >> (BITS(n) -  n)));
}
inline uint32 ROTR(uint32 n, uint32 x)
{
	assert(n < 32);
	return ((x >> n) | (x << (BITS(n) - n)));
}

inline void initializeHashValue(uint32 H[5])
{
	H[0] = 0x67452301;
	H[1] = 0xEFCDAB89;
	H[2] = 0x98BADCFE;
	H[3] = 0x10325476;
	H[4] = 0xC3D2E1F0;
}

inline uint32 K(t)
{
	static uint32 values[] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };
	assert(t < 80);
	if (t < 20)
		return values[0];
	else if (t < 40)
		return values[1];
	else if (t < 60)
		return values[2];
	return values[3];

}

/*!
 * @param	message		The message we want to pad
 * @param	message_len	The length of param message in bytes
 * @param	max_size	The number of bytes needed for the algorithms maximum message
 * @param	block_size	Block size of the algorithm in bytes
 * @param	buffer		Dynamically allocates memory for the buffer. Caller must free it
 */
int padding(const char *message, uint64 message_len, uint32 max_size, uint32 block_size, char **buffer, uint32 *len)
{
	assert(message != NULL);
	assert(buffer != NULL);
	assert(len != NULL);

	*buffer = NULL;
	*len = ((message_len + 1 + max_size) / block_size + 1) * block_size;
	*buffer = (char*)malloc(*len);
	if (buffer == NULL) {
		*len = 0;
		return 1;
	}

	char *p = *buffer;
	uint32 zero_bytes;	// number of bytes that will be zero
	uint64 length = 0;

	zero_bytes = *len - message_len - 1 - max_size;

	memcpy(p, message, message_len);
	p += message_len;

	*p++ = 0200;	// adds 1 bit at the end

	memset(p, '\0', zero_bytes); // converting bits to number of bytes == 0
	p += zero_bytes;

	/* This requires an explination, because you may be confused as to why we are changing endian here.
	 * There are two endian switches required. Since we hold message
	 * length as 64-bit, we switch this to big-endian notation.
	 * Then again when we iterate through the padded message in 32-bit
	 * blocks, so we can get back the length of the message. Otherwise
	 * we get something back in big-endian notation, which will be an
	 * entirely different number.
	 */
	length = endian64(message_len * 8);
	memcpy(p, &length, sizeof(length));

	return 0;
}

int sha1_hash_computation(const char *M, uint32 H[5], void callback(uint32, uint32, uint32, uint32, uint32, uint32))
{
	uint32 a, b, c, d, e;
	uint32 t;
	const uint32 *Mp = (const uint32*)M;
	
	a = H[0];
	b = H[1];
	c = H[2];
	d = H[3];
	e = H[4];
	uint32 W[80];
	for (t = 0; t < 80; t++)
	{
		if (t < 16) {
			W[t] = endian32(Mp[t]);
		} else {
			W[t] = ROTL(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
		}
		uint32 T = ROTL(5, a) + f(t, b, c, d) + e + K(t) + W[t];
		e = d;
		d = c;
		c = ROTL(30, b);
		b = a;
		a = T;
		if (callback)
			callback(t,a,b,c,d,e);
	}
	H[0] = a + H[0];
	H[1] = b + H[1];
	H[2] = c + H[2];
	H[3] = d + H[3];
	H[4] = e + H[4];

	//print("%X %X %X %X %X\n", H[0], H[1], H[2], H[3], H[4]);

	return 0;
}

#if 0
int main(void)
{
	const char *message = "abc";
	uint64 tmp;

	assert(sizeof(uint8) == 1);
	assert(sizeof(uint16) == 2);
	assert(sizeof(uint32) == 4);
	assert(sizeof(uint64) == 8);

	uint64 message_len = strlen(message);
	// 512 is block size. +64 is maximum length of message that is appended at the end of the message
	uint16 allocated = (512 * ((message_len * 8 + 64) / 512 + 1)) / 8;
	char *M = NULL;
	uint32 len = 0;
	/*
	uint64 l = message_len * 8;
	uint64 k = (448) - (l%448);
	print("l=%llu k=%llu\n", l, k);
	assert(k <= 448);
	char *M = malloc((l + k + 64) / 8);
	char *p = M;
	if (M == NULL) {
		print("OOM\n");
		return 1;
	}
	memcpy(p, message, message_len);
	p += message_len;
	*p++ = 0200;
	memset(p, 0, (k - 7) / 8); // converting bits to number of bytes == 0
	p += (k - 7) / 8;
	fprintf(stderr, "At byte %d\n", p - M);
	convertToLittleEndian(&message_len, sizeof(message_len), &tmp);
	memcpy(p, &tmp, sizeof(tmp));
	*/
	
	padding(message, message_len, 8, 64, &M, &len);
	print_hex(M, allocated);
	print_bin(M, allocated);

	int i = 0;
	uint32 a, b, c, d, e;
	for (; i < 1 /*(len / sizeof(uint32)) */; ++i)
	{
		a = H[0];
		b = H[1];
		c = H[2];
		d = H[3];
		e = H[4];
		uint32 t;
		uint32 *W = (uint32*)M + i;
		for (t = 0; t < 80; t++)
		{
			uint32 Wt;
			if (t < 16)
				Wt = W[t];
			else
				Wt = ROTL(1, W[t-3] ^ W[t-8] ^ W[t - 14] ^ W[t - 16]);
			uint32 T = ROTL(5, a) + f(t, b, c, d) + e + K(t) + endian32(Wt);
			e = d;
			d = c;
			c = ROTL(30, b);
			b = a;
			a = T;
			print("t=%d a=%X b=%X c=%X d=%X e=%X W=%X\n", t, a, b, c, d, e, endian32(Wt));
		}
		H[0] = a + H[0];
		H[1] = b + H[1];
		H[2] = c + H[2];
		H[3] = d + H[3];
		H[4] = e + H[4];
	}
	
	return 0;
}
#endif
