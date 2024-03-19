# Linux OpenMovment Makefile
# Original version by Ranjeet Singh

CC = gcc
CFLAGS = -w 
CFILES = main.c clear.c convert.c deploy.c download.c downloadmem.c record.c test.c verify.c 
INCLUDES = -I../include 
LIBS = -L../src -lomapi -lm -lpthread

ifneq ($(OS),Windows_NT)
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LIBS += -ludev
	endif
	ifeq ($(UNAME_S),Darwin)
		# -framework CoreFoundation
		LIBS += -framework Cocoa -framework IOKit -framework DiskArbitration
	endif
endif

omapi-examples : ../src/libomapi.a $(CFILES)
	$(CC) $(CFLAGS) $(INCLUDES) $(CFILES) -o omapi-examples -L. $(LIBS) 

../src/libomapi.a : ../src/*.c ../src/*.h
	make -C ../src

clean:
	rm -f *.o omapi-examples
