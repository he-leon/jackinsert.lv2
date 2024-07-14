CC = gcc

CFLAGS = -fvisibility=hidden -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread $(pkg-config --cflags lv2) -lm $(pkg-config --libs lv2)
TARGET = jackinsert.so

BINDIR = .

PREFIX  ?= /usr/local
DESTDIR ?=
LIBDIR  ?= /lib/lv2/

SRC = jackinsert.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BINDIR)/$@

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)$(LIBDIR)jloop.lv2
	install -m 644 *.so  $(DESTDIR)$(PREFIX)$(LIBDIR)jloop.lv2/
	install -m 644 *.ttl $(DESTDIR)$(PREFIX)$(LIBDIR)jloop.lv2/

clean:
	rm -rf $(OBJDIR) $(BINDIR)/$(TARGET)

.PHONY: all clean
