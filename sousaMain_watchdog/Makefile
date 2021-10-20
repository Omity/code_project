CC=/home/GJ/Desktop/Tina_Linux/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc-6.4.1
TARGET=watchdog
CP_PATH=../../../allwinner/SousaApp/app/

ALL: $(TARGET)

$(TARGET):
	$(CC) -Wall -g sousaMain_watchdog.c -o $(TARGET)
	# mv $(TARGET) $(CP_PATH)/$(TARGET)

.PHONY: clean

clean:
	rm -f $(TARGET) $(CP_PATH)/$(TARGET)


