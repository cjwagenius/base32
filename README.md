# base32
A small efficient public domain base32 encoder/decoder in ANSI C.

```
size_t b32check(const char *src, size_t src_len)

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
```

```
size_t b32encode(char *dst, const void *src, size_t src_len)

	/**
	 * Encodes data of [len] length from [src] to [dst]. if [len] == -1,
	 * [src] is assumed to be a null-terminated string and strlen() will be
	 * used to calculate its length. [dst] must have enough space to hold
	 * the encoded data. b32enclen() may be used to calculate the space
	 * needed.
	 *
	 * Returns the length of the b32-encoded string.
	 **/
```

```
size_t b32decode(void *dst, const char *src, size_t src_len)

	/**
	 * Decodes a base32-encoded string of [len] bytes from [src] to [dst].
	 * If [len] == -1, [src] is assumed to be a null-terminated string, and
	 * strlen() will be used to calculate its length. [dst] must have
	 * enough space to hold the decoded data. A blunt calculation to
	 * guarantee space for it, is ([len] / 8 + 1) * 5). Use b32check() to
	 * get exact precision.
	 * This function doesn't check for errors. An invalid b32-string will
	 * generate undefined results. Please use b32check() if validation is
	 * required.
	 *
	 * Returns the length of the data in [dst].
	 **/
```

```
size_t b32enclen(size_t len)

	/**
	 * Calculates the space needed to b32-encode data of size [len].
	 **/
```

