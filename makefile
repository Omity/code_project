LIBS = -lm
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -Wall
OBJ = $(patsubst %.c, %, $(SRC))

ALL: $(OBJ)

$(OBJ): $(OBJS)
	gcc $< -o $@

$(OBJS): $(SRC)
	gcc -c $< -o $@

clean: 
	rm -f $(OBJS) $(OBJ)

.PHONY: clean ALL
