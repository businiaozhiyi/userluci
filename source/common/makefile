target = Common

src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c,./%.o, $(src))


$(shell mkdir $(TOP_PATH)/build/obj/$(target))

all: $(obj)  
#	$(CC) $(TOP_PATH)/build/obj/$(target)/$(obj) -o $(TOP_PATH)/build/bin/$(target)

%.o: %.c  
	$(CC) $(CFLAGS) -c $< -o $(TOP_PATH)/build/obj/$(target)/$@

.PHONY: clean  
clean:  
	rm -rf $(TOP_PATH)/build/obj/$(target) 
