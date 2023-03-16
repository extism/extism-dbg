LDFLAGS+= -lextism
CFLAGS+= -I/usr/local/include
PREFIX?=/usr/local

bin:
	$(CC) -g -Wall -o extism-dbg extism-dbg.c $(CFLAGS) $(LDFLAGS)

install:
	install extism-dbg $(PREFIX)/bin/extism-dbg
	install extism-lldb $(PREFIX)/bin/extism-lldb
	install extism-perf $(PREFIX)/bin/extism-perf

uninstall:
	rm -f $(PREFIX)/bin/extism-dbg $(PREFIX)/bin/extism-lldb $(PREFIX)/bin/extism-perf

check:
	./extism-dbg ./count-vowels.wasm count_vowels "this is a test" | grep 4

