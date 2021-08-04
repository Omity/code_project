LIBS = -lm
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -Wall
OBJ = $(patsubst %.c, %, $(SRC))
CC = gcc

ALL: $(OBJ)

%:%.o 
	$(CC) $< $(CFLAGS) -o $@ 

%.o:%.c
	$(CC) -c $< $(CFLAGS) -o $@
clean: 
	$(foreach TARGET, $(OBJS), rm -f $(TARGET))
	$(foreach TARGET, $(OBJ), rm -f $(TARGET))

up:
	git add .
	git commit -m "c语言作业"
	git push origin c_100_example

.PHONY: clean ALL
