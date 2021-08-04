LIBS = -lm
SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))
CFLAGS = -Wall

ALL: output.out

output.out: $(OBJS)
	gcc $< -o $@  

$(OBJS): $(SRC)
	gcc -c $< -o $@

clean: 
	rm -f $(OBJS) output.out

.PHONY: clean ALL
