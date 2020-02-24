
CC = /usr/bin/cc
CFLAGS = -Wall -g -Wextra -march=native -mtune=native -O3 -fomit-frame-pointer 
NISTFLAGS = -O3 -fomit-frame-pointer -march=native -fPIC
SOURCES = 	main.c \
			Alg.c \
			fips202.c \
			cpucycles.c \
			speed.c \
			randombytes.c \
			Polynomial.c \
			io.c \
			Rounding.c \
			random.c \

			
HEADERS = 	randombytes.h \
			fips202.h \
			cpucycles.h \
			speed.h \
			Parameters.h \
			Alg.h \
			Polynomial.h \
			io.h \
			Rounding.h \
			random.h \


all: main

main: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

.PHONY: clean

clean:
	rm -f main
