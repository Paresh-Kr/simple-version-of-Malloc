CC=gcc
CXX=g++
CFLAGS = -g -Wall -Wextra -pedantic
DEBUG=\
 -DCS550_ASSERT \
 -DCS550_DBG_PRINT \

.PHONY: a.out

a.out:
	$(CC)  $(DEBUG) $(CFLAGS) -std=c99 -DCS550_RENAME -D_XOPEN_SOURCE=500 test.c malloc.c
clean:
	rm -f  a.out
