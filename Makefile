
CFLAGS+=-Wall -pedantic -std=c99

base32.so: base32.h
	$(CC) -DBASE32_DEFINE -fPIC -o$@ -shared -xc $(CFLAGS) $^
	python test_base32.py

