CC      := gcc
CFLAGS  := -Wall -Werror -g
LDFLAGS := 
LIBS    := 
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	LIBS += -luuid
endif

SOURCES = udp.c udpping.c beacon.c
OBJS    = $(SOURCES:%.c=%.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o udpping $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o udpping
