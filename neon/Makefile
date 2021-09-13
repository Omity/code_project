# 测试文件名称
CC=arm-openwrt-linux-gcc
TARGET=neon_test
# OBJ_FILE=./neon_instr.o 
OBJ_FILE=./neon_float.o
LDFLAGS=-g -O3 -mfloat-abi=softfp -mfpu=neon -ffast-math 
FLAGS=-lpthread -static

all:$(TARGET)

# neon_instr.o:neon_instr.c 
neon_float.o:neon_float.c
	$(CC) $(LDFLAGS) -c $< -o $@

$(TARGET):$(OBJ_FILE)
	$(CC) -o $(TARGET) $(OBJ_FILE)  $(FLAGS)  
  
clean:
	rm -rf $(TARGET) *.o


