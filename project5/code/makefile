CC=gcc
CFLAGS=-I.
DEPS = tools.h
OBJ=bmp_tool.o main.o

main.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY:
run:
	./main.exe
