CFLAGS=-Wall -O3 -s
ARFLAGS=-r -s

.PHONY: all
.PHONY: install
.PHONY: clean
.PHONY: love

all: libpicel.a
libkina.a: bitmap.o palette.o drawing.o property.o stream.o \
loadpng.o savepng.o
	ar $(ARFLAGS) $@ $^

bitmap.o: bitmap.c picel.h bitmap.h
drawing.o: drawing.c picel.h bitmap.h
palette.o: palette.c picel.h bitmap.h
property.o: property.c picel.h bitmap.h
stream.o: stream.c picel.h
loadpng.o: loadpng.c picel.h bitmap.h
savepng.o: savepng.c picel.h bitmap.h

install:
	cp ./picel.h /usr/include
	cp ./libpicel.a /usr/lib

clean:
	rm -rf ./libpicel.a
	rm -rf ./*.o

love:
	@echo not war?
