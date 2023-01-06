LDFLAGS+= -lextism
CFLAGS+= -I/usr/local/include
PREFIX?=/usr/local

bin:
	$(CC) -g -o extism-dbg extism-dbg.c $(CFLAGS) $(LDFLAGS)

install:
	install extism-dbg "$PREFIX"/bin/extism-dbg
	install extism-lldb "$PREFIX"/bin/extism-lldb

uninstall:
	rm -f "$PREFIX"/bin/extism-dbg "$PREFIX"/bin/extism-lldb 
