#获取子目录深度为1的所有目录名称
#dirs := $(shell find . -maxdepth 1 -type d)
dirs := ./common
dirs += $(shell find . -path "./Common" -prune -o -type d -print)
dirs := $(basename $(patsubst ./%,%,$(dirs)))
dirs := $(filter-out $(exclude_dirs),$(dirs))

SUBDIRS:=$(dirs)

all:
#	@echo $(SUBDIRS)
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir all || exit 1;\
	done
clean:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir clean || exit 1;\
	done
