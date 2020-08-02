MINGW=d:/home/dev/mingw64
LUA_INC=-I$(MINGW)/include
LUA_LIB=-I$(MINGW)/bin -llua54
linux:
	make goap.so "DLLFLAGS= -shared -fPIC"
macosx:
	make goap.so "DLLFLAGS= -bundle -undefined dynamic_lookup"

goap.so: goap.c lgoap.c
	env gcc -O2 -Wall $(DLLFLAGS) -o $@ $^
goap.dll: goap.c lgoap.c
	gcc -g -Wall -fPIC --shared -o $@ $^ $(LUA_INC) $(LUA_LIB)
