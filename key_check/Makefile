# 测试文件名称
CC=arm-openwrt-linux-gcc
TARGET=key_test
OBJ_FILE=./key_test.o ./uart_data_deal.o ./uart_phy.o ./key_phare.o ./uart_queue.o
FLAGS=-lpthread 


$(TARGET):$(OBJ_FILE)
	$(CC) -o $(TARGET) $(OBJ_FILE)  $(FLAGS)  
  
clean:
	rm -rf $(TARGET) *.o


