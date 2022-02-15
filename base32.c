/**
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 *
 * Written and made public by C.J.Wagenius - 2020-03-11
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B32(c)		     (c < 'A' ? c - 24 : c - 'A')
/* dpdmap - maps number of non-padding b32-sequences to number of bytes */
static const char dpdmap[] = { 0, 0, 1, 0, 2, 3, 0, 4, 5 };
/* padmap - maps number of bytes to number of non-padding b32-sequences */
static const char padmap[] = { 0, 2, 4, 5, 7, 8 };
static const char b32map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

/* 11111 111 | 11 11111 1 | 1111 1111 | 1 11111 11 | 111 11111 */

#ifndef NOB32CHECK
/** checks a b32 encoded string for errors and returns decoded length.
 *
 * if <src_len> is -1, <src> is assumed to be a null-terminated string.
 *
 * returns:
 * 	>0	required length of the decoded buffer on success
 * 	-1	if a invalid b32-character was found.
 * 	-2	if the b32 string length is not is a muliplier of eight.
 *
 **/
size_t
b32check(const char *src, size_t src_len)
{
	size_t i;
	size_t pad = 0;

	if (src_len == -1)
		src_len = strlen(src);
	if (src_len % 8)
		return -2;
	
	for (i = 0; i < src_len; i++) {
		if (!(src[i] >= '2' && src[i] <= '7') &&
		    !(src[i] >= 'A' && src[i] <= 'Z') && !pad) {
			if (src[i] != '=')
				return -1;
			if (!pad) pad = i;
		}
	}

	return dpdmap[pad % 8]; 
}
#endif /* NOB32CHECK */

static size_t
b32dec(void *dst, size_t nbytes, const char *src, size_t src_len)
{
	int b32len;
	unsigned char *d = dst;

	for (b32len = 0; b32len < 8 && src[b32len] != '='; b32len++);

	if (d) {
		switch (b32len) {
		case 8: case 7:
			d[4] = B32(src[6]) << 5 | B32(src[7]);
		case 6: case 5:
			d[3] = B32(src[4]) << 7 | B32(src[5]) << 2 |
			       B32(src[6]) >> 3;
		case 4:
			d[2] = B32(src[3]) << 4 | B32(src[4]) >> 1;
		default:
			d[1] = B32(src[1]) << 6 | B32(src[2]) << 1 |
			       B32(src[4]) >> 4;
			d[0] = B32(src[0]) << 3 | B32(src[1]) >> 2;
		}
	}

	nbytes += dpdmap[b32len];
	if (src_len > 8)
		return b32dec(d + 5, nbytes, src + 8, src_len - 8);

	return nbytes;
}

/* returns required length for <dst>. <dst> may be null. */
size_t
b32decode(void *dst, const char *src, size_t src_len)
{
	if (src_len == -1)
		src_len = strlen(src);

	return b32dec(dst, 0, src, src_len);
}

static size_t
b32enc(char *dst, size_t nbytes, const void *src, size_t src_len)
{
	int i;
	int b32;
	const unsigned char *s = src;
	int bytes = src_len > 5 ? 5 : src_len;

	if (dst) {
		switch (bytes) {
		case 5:
			b32 = (s[4] & 0x1f);
			dst[7] = b32map[b32];
		case 4:
			b32 = (s[3] & 0x03) << 3 | (s[4] & 0xe0) >> 5;
			dst[6] = b32map[b32];
			b32 = (s[3] & 0x7c) >> 2;
			dst[5] = b32map[b32];
		case 3:
			b32 = (s[2] & 0x0f) << 1 | (s[3] & 0x80) >> 7;
			dst[4] = b32map[b32];
		case 2:
			b32 = (s[1] & 0x01) << 4 | (s[2] & 0xf0) >> 4;
			dst[3] = b32map[b32];
			b32 = (s[1] & 0x3e) >> 1;
			dst[2] = b32map[b32];
		default:
			b32 = (s[0] & 0x07) << 2 | (s[1] & 0xc0) >> 6;
			dst[1] = b32map[b32];
			b32 = (s[0] & 0xf8) >> 3;
			dst[0] = b32map[b32];
		}
		for (i = padmap[bytes]; i < 8; i++)
			dst[i] = '=';
	}

	if (src_len <= 5)
		return nbytes + 8;

	return b32enc(dst + 8, nbytes + 8, s + bytes, src_len - bytes);
}

size_t
b32encode(char *dst, const void *src, size_t src_len)
{
	if (src_len == (size_t)-1)
		src_len = strlen(src);

	return b32enc(dst, 0, src, src_len);
}

