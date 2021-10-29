
ROOT_NAME="zynq_zturn/"
UBOOT_NAME="uboot/"
KERNEL_NAME="kernel/"
FS_NAME="rootfs/"
SH_PATH=$(dirname $0)
ROOT_PATH=$(cd $SH_PATH && cd .. && pwd)
CODE=0           # 本用户密码,如切换用户或者虚拟机需更改

function root_file()
{
	if [ ! -x "$ROOT_PATH/out" ] ; then
		mkdir "$ROOT_PATH/out"
		if [ ! -x "$ROOT_PATH/out/$ROOT_NAME" ] ; then
			mkdir -m 777 -vp "$ROOT_PATH/out/$ROOT_NAME"{"$UBOOT_NAME","$KERNEL_NAME","$FS_NAME"}
		fi
	fi
}

function get_uboot()
{
	local src=$1
	local target="$src/out/$ROOT_NAME$UBOOT_NAME"
	local src_file="u-boot"
	local target_file="u-boot.elf"
	if [ -d "$target" ] ; then
		cp "$src_file" "$target/$target_file"
	fi
}

function build_boot()
{
	echo "start building u-boot"
	local T="$ROOT_PATH/u-boot-xlnx"
	if [ -d "$T" ] ; then
		cd $T && ./build.sh && get_uboot $(cd .. && pwd)
		echo "#################################################"
		echo "################## uboot OK #####################"
		echo "#################################################"
	fi
	cd $ROOT_PATH
}

function get_kernel()
{
	local src=$1
	local target="$src/out/$ROOT_NAME$KERNEL_NAME"
	local src_file="arch/arm/boot/uImage"
	local src_dts="arch/arm/boot/dts/zynq-zturn.dts"
	local target_file="uImage"
	local target_dts="zynq-zturn.dts"
	if [ -d "$target" ] ; then
		cp "$src_file" "$target/$target_file"
		cp "$src_dts" "$target/$target_dts"
	fi
}

function build_kernel()
{
	echo "start building kernel"
	local T="$ROOT_PATH/linux-xlnx"
	if [ -d "$T" ] ; then
		cd $T && ./build.sh && get_kernel $(cd .. && pwd)
		echo "#################################################"
		echo "################## kernel OK ####################"
		echo "#################################################"
	fi
	cd $ROOT_PATH
	
}

function ramdisk_amend()
{
	echo "ramdisk amend ..."
}

function get_ramdisk()
{
	local T="$ROOT_PATH/out/$ROOT_NAME$FS_NAME"
	local file="uramdisk.image.gz"
	if [ -x "$T" ] ; then
		cp $file "$T/$file"
	fi
}

function build_fs()
{
	echo "start building ramdisk"
	local T="$ROOT_PATH/Filesystem"
	if [ -d "$T" ] ; then
		cd $T
		if [ ! -x "tmp" ] ; then
			mkdir tmp 
		fi
		cp uramdisk.image.gz tmp/
		cd tmp/
		# 去掉mkimage 生成的64 bytes 的文件头，生成新的ramdisk.image.gz
		dd if=uramdisk.image.gz of=ramdisk.image.gz bs=64 skip=1
		gunzip ramdisk.image.gz
		mkdir -p rootfs
		if [ ${USER} != "root" ] ;then
			sudo mount -o loop,rw ramdisk.image rootfs << EOF
			${CODE}
EOF
			## 添加修改
			ramdisk_amend
			##
		else
			mount -o loop,rw ramdisk.image rootfs
			## 添加修改
			ramdisk_amend
			##
		fi
		sync    # 同步修改
		if [ ${USER} != "root" ] ;then
			sudo umount rootfs << EOF
			${CODE}
EOF
		else
			umount rootfs
		fi
		gzip -9 ramdisk.image
		mkimage -A arm -T ramdisk -C gzip -n Ramdisk -d ramdisk.image.gz uramdisk.image.gz
		get_ramdisk
		rm -f -R ../tmp
		echo "#################################################"
		echo "################## ramdisk OK ###################"
		echo "#################################################"
	fi
	cd $ROOT_PATH
}

function part_boot_copy()
{
	echo "更改BOOT分区的BOOT.bin, uImage和uramdisk.image.gz"
	local S=$1
	local T="$ROOT_PATH/out/$ROOT_NAME"
	local uboot="uboot/BOOT.bin"   # BOOT.bin需要结合逻辑的elf文件,所有需要外部生成,然后放入uboot文件夹下
	local kernel="kernel/uImage"
	local fs="rootfs/uramdisk.image.gz"
	if [ ${USER} != "root" ] ; then
		if [ -f "$uboot" ] ; then 
			sudo cp "$T$uboot" $S
		fi
		sudo cp "$T$kernel" $S
		sudo cp "$T$fs" $S
	else
		if [ -f "$uboot" ] ; then 
			cp "$T$uboot" $S
		fi
		cp "$T$kernel" $S
		cp "$T$fs" $S
	fi
}

function part_fs_amend()
{
	echo "修改文件系统..."
}

function get_sd()
{
	echo "复制镜像"
	local T="$ROOT_PATH/out/$ROOT_NAME"
	local S=$1
	local file=$2
	if [ -f "$S/$file" ] ; then
		cp "$S/$file" "$T$file"
	fi
}

function build_sd()
{
	echo "start making sdImage"
	local T="$ROOT_PATH/Filesystem"
	local file_zip="xillinux-1.3.img.gz"
	local file_unzip="xillinux-1.3.img"
	if [ -x "$T" ] ; then
		cd $T
		if [ ! -d "sdimage" ] ; then
			mkdir sdimage
			cp $file_zip sdimage/
			cd sdimage
			echo "文件过大, 解压缩时间可能有点长,请耐心等待..."
			gunzip $file_zip
			if [ $? -eq 0 ] ; then
				echo "解压缩成功"
			else
				echo "解压缩失败"
				exit 1
			fi
		else
			cd sdimage
		fi
		if [ -f "$file_unzip" ] ; then
			local img1_offset=$(fdisk -l -u $file_unzip | awk '$1=="xillinux-1.3.img1"{print $3}')
			local img2_offset=$(fdisk -l -u $file_unzip | awk '$1=="xillinux-1.3.img2"{print $2}')
			local img1=$((10#${img1_offset}*512))
			local img2=$((10#${img2_offset}*512))
			mkdir -p part0 part1
			# 挂载BOOT分区
			if [ ${USER} != "root" ] ; then
				sudo mount -o rw,offset=$img1 xillinux-1.3.img part0/ << EOF
				${CODE}
EOF
			else
				mount -o rw,offset=$img1 xillinux-1.3.img part0/
			fi
			# 修改BOOT分区
			part_boot_copy "$pwd/part0"
			
			# 挂载文件系统分区
			if [ ${USER} != "root" ] ; then
				sudo mount -o rw,offset=$img2 xillinux-1.3.img part1/ << EOF
				${CODE}
EOF
			else
				mount -o rw,offset=$img2 xillinux-1.3.img part1/
			fi
			# 修改文件系统分区
			part_fs_amend
			# 同步
			sync
			if [ ${USER} != "root" ] ; then
				sudo umount part0 part1
			else
				umount part0 part1
			fi
			# 复制镜像
			get_sd "$pwd" "$file_unzip"
		fi
		rm -f -R ../sdimage
		echo "#################################################"
		echo "################## sdimage OK ###################"
		echo "#################################################"
	fi
	cd $ROOT_PATH	
}

function main()
{
	root_file
	build_boot
	build_kernel
	build_fs
	build_sd
}

## main ##
main

