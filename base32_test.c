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

#include "base32.c"

#include <assert.h>
#include <string.h>

int
main(int argc, char **argv)
{
	char str[] = "abcde\0fgh";
	char tst1[] = "MFRGGZDF";
	char tst2[] = "MZTWQ===";
	char tst3[] = "MFRGGZDFABTGO2A=";
	char buf[sizeof(tst3)];

	memset(buf, 0, sizeof(buf));
	assert(b32enc(buf, 0, str, 5) == 8);
	assert(!strncmp(buf, tst1, 8));

	memset(buf, 0, sizeof(buf));
	assert(b32enc(buf, 0, str+6, 3) == 8);
	assert(!strncmp(buf, tst2, 8));

	memset(buf, 0, sizeof(buf));
	assert(b32enc(buf, 0, str, 9) == 16);
	assert(!strncmp(buf, tst3, 16));

	memset(buf, 0, sizeof(buf));
	assert(b32dec(buf, 0, tst1, 8) == 5);
	assert(!strncmp(buf, str, 5));

	memset(buf, 0, sizeof(buf));
	assert(b32dec(buf, 0, tst2, 8) == 3);
	assert(!strncmp(buf, str+6, 3));

	memset(buf, 0, sizeof(buf));
	assert(b32dec(buf, 0, tst3, 16) == 9);
	assert(!strncmp(buf, str, 9));

	assert(b32check("A9F", -1) == -2);
	assert(b32check("A9F=====", -1) == -1);
	assert(b32check(tst2, -1) == 3);

	return EXIT_SUCCESS;
}

