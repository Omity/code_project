SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
OBJS = $(patsubst %.c , %, $(SRC)) 


ALL: $(OBJS)

$(OBJS): $(OBJ)
	gcc $(OBJ) -o $(OBJS)
$(OBJ): $(SRC)
	gcc -c $(SRC) -o $(OBJ)
