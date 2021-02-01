# Linux OpenMovment Makefile
# Original version by Ranjeet Singh

CC = gcc
CFLAGS = -w 
CFILES = main.c clear.c convert.c deploy.c download.c downloadmem.c record.c test.c verify.c 
INCLUDES = -I../include 
LIBS = -L../src -lomapi -ludev -lm -lpthread

omapi-examples : ../src/libomapi.a $(CFILES)
	$(CC) $(CFLAGS) $(INCLUDES) $(CFILES) -o omapi-examples -L. $(LIBS) 

../src/libomapi.a : ../src/*.c ../src/*.h
	make -C ../src  # (cd ../src && make)
