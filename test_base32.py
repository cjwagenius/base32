
import base64
import random
from ctypes import *

BUFLEN = 512*1024

def main():

    base32 = CDLL("./base32.so")
    dta = random.randbytes(BUFLEN)
    p32 = base64.b32encode(dta)
    d32 = b'\0' * BUFLEN
    e32 = b'\0' * base32.b32enclen(BUFLEN)

    assert(len(e32) == len(p32))
    assert(base32.b32encode(c_char_p(e32), c_char_p(dta), len(dta)) == len(p32))
    assert(e32 == p32)
    assert(base32.b32decode(c_char_p(d32), c_char_p(e32), len(e32)) == BUFLEN)
    assert(d32 == dta)


if __name__ == "__main__":

    main()

