target = 4G

src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c,./%.o, $(src))

bulid_Common_obj:=$(shell find $(TOP_PATH)/build/obj/common -name "*.o*")
bulid_cgic_obj:=$(shell find $(TOP_PATH)/build/obj/cgic -name "*.o*")
bulid_cJSON_obj:=$(shell find $(TOP_PATH)/build/obj/cJSON -name "*.o*")


$(shell mkdir $(TOP_PATH)/build/obj/$(target))

all: $(obj)  
	$(CC) $(CFLAGS) $(LDFLAGS) $(bulid_Common_obj) $(bulid_cgic_obj) $(bulid_cJSON_obj) $(TOP_PATH)/build/obj/$(target)/$(target).o -o $(TOP_PATH)/build/bin/$(target)
%.o: %.c  
	$(CC) $(CFLAGS) -c $< -o $(TOP_PATH)/build/obj/$(target)/$@

.PHONY: clean  
clean:  
	rm -rf $(TOP_PATH)/build/obj/$(target) $(TOP_PATH)/build/bin/$(target) ./*.map
