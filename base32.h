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

#ifndef BASE32_H
#define BASE32_H
#include <stddef.h>

#ifndef NOB32CHECK
size_t b32check(const char *src, size_t len);
#endif /* NOB32CHECK */
extern size_t b32decode(void *dst, const char *src, size_t len);
extern size_t b32encode(char *dst, const void *src, size_t len);
extern size_t b32enclen(size_t len);

#ifdef BASE32_DEFINE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B32(c)		     (c < 'A' ? c - 24 : c - 'A')
/* dpdmap - maps number of non-padding b32-sequences to number of bytes */
static const char dpdmap[] = { 5, 0, 1, 0, 2, 3, 0, 4, 5 };
/* padmap - maps number of bytes to number of non-padding b32-sequences */
static const char padmap[] = { 0, 2, 4, 5, 7, 8 };
static const char b32map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

#ifndef NOB32CHECK
size_t b32check(const char *src, size_t len)
{
	/**
	 * Checks a base32-encoded string and return the length of the string
	 * in a decoded format. If [len] is (size_t)-1, str is assumed to be a
	 * null-terminated string.
	 *
	 * Returns 0 on success, and a negative value on error. Errno is set
	 * to indicate what went wrong.
	 *
	 * Errors:
	 *	ERANGE		The string length is not a multipler of eight
	 *			and is therefore not a valid b32-string. 
	 *	EILSEQ		An illegal byte for base32 was found.
	 **/

	size_t pad = 0;

	errno = 0;
	if (len == -1)
		len = strlen(src);
	if (len % 8) {
		errno = ERANGE;
		return -1;
	}
	
	for (size_t i = 0; i < len; i++) {
		if (pad) {
			if (src[i] != '=') {
				errno = EILSEQ;
				return -1;
			}
		} else {
			if (!(src[i] >= '2' && src[i] <= '7') &&
			    !(src[i] >= 'A' && src[i] <= 'Z')) {
				if (src[i] != '=') {
					errno = EILSEQ;
					return -1;
				}
				pad = i;
			}
		}
	}

	return ((len / 8) * 5) - (5 - dpdmap[pad % 8]); 
}
#endif /* NOB32CHECK */

static unsigned b32dec(void *dst, const char *src, size_t len)
{
	int nb;
	unsigned char **d = dst;

	nb = len > 7 ? 8 : len;
	while (src[nb - 1] == '=') nb --;

	switch (nb) {
	case 8: case 7:
		(*d)[4] = B32(src[6]) << 5 | B32(src[7]);
	case 6: case 5:
		(*d)[3] = B32(src[4]) << 7 | B32(src[5]) << 2 |
		       B32(src[6]) >> 3;
	case 4:
		(*d)[2] = B32(src[3]) << 4 | B32(src[4]) >> 1;
	default:
		(*d)[1] = B32(src[1]) << 6 | B32(src[2]) << 1 |
		       B32(src[3]) >> 4;
		(*d)[0] = B32(src[0]) << 3 | B32(src[1]) >> 2;
	}
	*d += dpdmap[nb];

	return len > 7 ? 8 : len;
}
size_t b32decode(void *dst, const char *src, size_t len)
{
	/**
	 * Decodes a base32-encoded string of [len] bytes from [src] to [dst].
	 * If [len] == -1, [src] is assumed to be a null-terminated string, and
	 * strlen() will be used to calculate its length. [dst] must have
	 * enough space to hold the decoded data. A blunt calculation to
	 * guarantee space for it, is ([len] / 8) * 5 + 1). Use b32check() to
	 * get exact precision.
	 * This function doesn't check for errors. An invalid b32-string will
	 * generate undefined results. Please use b32check() if validation is
	 * required.
	 *
	 * Returns the length of the data in [dst].
	 **/

	char *zst = dst;

	if (len == -1)
		len = strlen(src);

	while (len) {
		len -= b32dec(&zst, src, len);
		src += 8;
	}

	return zst - (char*)dst;
}

static unsigned b32enc(char *dst, const void *src, size_t nb)
{
	int b32;
	const char pad[] = "========";
	const unsigned char *s = src;

	if (nb > 5) nb = 5;
	switch (nb) {
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
	if (nb < 5) {
		int i = padmap[nb];
		strcpy(dst + i, pad + i);
	}

	return nb;
}
size_t b32encode(char *dst, const void *src, size_t len)
{
	/**
	 * Encodes data of [len] length from [src] to [dst]. if [len] == -1,
	 * [src] is assumed to be a null-terminated string and strlen() will be
	 * used to calculate its length. [dst] must have enough space to hold
	 * the encoded data. b32enclen() may be used to calculate the space
	 * needed.
	 *
	 * Returns the length of the b32-encoded string.
	 **/

	char *zst = dst;
	const char *zrc = src;

	if (len == (size_t)-1)
		len = strlen(zrc);

	while (len) {
		len -= b32enc(zst, zrc, len);
		zrc += 5;
		zst += 8;
	}

	return zst - dst;
}
size_t b32enclen(size_t len)
{
	/**
	 * Calculates the space needed to b32-encode data of size [len].
	 **/

	size_t r;

	r = len / 5;
	r += len % 5 ? 1 : 0;
	r *= 8;

	return r;
}
#endif /* BASE32_DEFINE */
#endif /* BASE32_H */
