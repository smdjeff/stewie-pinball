UNAME := $(shell uname)
ifeq ($(UNAME),$(filter $(UNAME),raspberrypi))
LDFLAGS += -pthread -lpigpio -lrt
else
ADD_OBJS = linux/portable.o
endif

src = $(wildcard *.c)
obj = $(src:.c=.o) $(ADD_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I. -o $@ -c $<

stewie: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) stewie



