CFLAGS=`pkg-config --cflags opencv`
LIBS=`pkg-config --libs opencv`
CC=gcc
test_565 : test_565.o
	$(CC) $(CFLAGS) test_565.o -o test_565 $(LIBS)
lut_test.o : test_565.c
	$(CC) $(CFLAGS) -c test_565.c $(LIBS)

clean :
	rm test_565 test_565.o
