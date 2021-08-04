LIBS = -lm
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -Wall
OBJ = $(patsubst %.c, %, $(SRC))
CC = gcc

ALL: $(OBJ)

$(OBJ):%:%.c 
	$(CC) $< $(CFLAGS) -o $@ 

clean: 
	$(foreach TARGET, $(OBJS), rm -f $(TARGET))
	$(foreach TARGET, $(OBJ), rm -f $(TARGET))

.PHONY: clean ALL
