
#编译选项
export CC=mips-openwrt-linux-gcc
export CFLAGS= -lm -Wall -ldl
CFLAGS+= -ffunction-sections -fdata-sections
export LDFLAGS= -L /home/utek/ut-9082/qca9531_mips_u2_notgit_luci/qsdk/qsdk/staging_dir/target-mips_r2_uClibc-0.9.33.2/usr/lib/lua/ -liwinfo
export LDFLAGS+= -L /home/utek/ut-9082/qca9531_mips_u2_notgit_luci/qsdk/qsdk/staging_dir/target-mips_r2_uClibc-0.9.33.2/usr/lib -luci
export LDFLAGS+= -L /home/utek/ut-9082/qca9531_mips_u2_notgit_luci/qsdk/qsdk/staging_dir/target-mips_r2_uClibc-0.9.33.2/usr/lib -lxml2

LDFLAGS+=-Wl,-Map=object.map,--cref,--gc-section
MAKE = make

#需要排除的目录
exclude_dirs:=build

#建立目录
export BUILD:=build
export TOP_PATH:=$(shell pwd)

#获取子目录深度为1的所有目录名称
dirs := $(shell find . -maxdepth 1 -type d)
dirs := $(basename $(patsubst ./%,%,$(dirs)))
dirs := $(filter-out $(exclude_dirs),$(dirs))

#获取头文件的目录
include_dir_s:=$(shell find ./source -maxdepth 1 -type d)
include_dir_s:= $(basename $(patsubst ./%,%,$(include_dir_s)))
include_dir_s := $(foreach dir,$(include_dir_s),-I $(TOP_PATH)/$(wildcard $(dir)))
CFLAGS += $(include_dir_s)

include_dir_c:=$(shell find ./components -maxdepth 1 -type d)
include_dir_c:= $(basename $(patsubst ./%,%,$(include_dir_c)))
include_dir_c:= $(foreach dir,$(include_dir_c),-I $(TOP_PATH)/$(wildcard $(dir)))
CFLAGS += $(include_dir_c)

CFLAGS += -I /home/utek/ut-9082/qca9531_mips_u2_notgit_luci/qsdk/qsdk/staging_dir/target-mips_r2_uClibc-0.9.33.2/usr/include/libxml2

#获取.o 文件目录
#bulid_obj:=$(shell find ./build/obj -name "*.o*")
#bulid_obj:= $(basename $(patsubst ./%,%,$(bulid_obj)))
#bulid_obj:= $(foreach dir,$(bulid_obj),-I $(TOP_PATH)/$(wildcard $(dir)))

#CFLAGS += $(bulid_obj)

SUBDIRS:=$(dirs)

all:
#	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir all || exit 1;\
	done
	@echo $(include_dirs)
	$(MAKE) -C components all
	$(MAKE) -C source  all
clean:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir clean || exit 1;\
	done

