# base32
A small efficient public domain base32 encoder/decoder in ANSI C.

```
size_t b32check(const char *src, size_t src_len)
```
>Checks if a base32 encoded string is correct and returns the required size of the buffer recieving the decoded data. If *src_len* is -1, *src* is assumed to be a null-terminated string. This function can be omitted at compile time with`-DNOB32CHECK`

```
size_t b32encode(char *dst, const void *src, size_t src_len)
```
>Encodes *src_len* bytes at *src* to *dst* (if *dst* is not NULL). Returns required buffer length for *dst*.

```
size_t b32decode(void *dst, const char *src, size_t src_len)
```
>Decodes *src_len* bytes at *src* to *dst* (if *dst* is not NULL). Returns required buffer length for *dst*. *b32decode()* does not check for errors. An invalid base32-string leads to undefined behavior. Use *b32check()* if validation is needed.



