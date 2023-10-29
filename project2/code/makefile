CC=gcc
CFLAGS=-I.
DEPS = tools.h
OBJ=bmp_tool.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)