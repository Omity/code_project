CC = arm-openwrt-linux-gcc
TARGET=neon_test
OBJ_FILE=./neon_sin.o
FLAGS=-g -o3 


$(TARGET):$(OBJ_FILE)
	$(CC) -o $(TARGET) $(OBJ_FILE)  $(FLAGS)  
  
clean:
	rm -rf $(TARGET) *.o
