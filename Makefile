GPP = clang++

CFLAGS = -w -std=c++2a -I /usr/local/include -I include -g -fsanitize=address -Wl #-O3 -march=native -mtune=native

ifeq ($(OS),Windows_NT)
	AR = llvm-ar
	RM = del 
	LIBNAME = fan_windows_clang.a
else
	CFLAGS += -DFAN_INCLUDE_PATH=/usr/include
	AR = ar
	RM = rm -f
	LIBNAME = fan.a
endif

ifeq ($(OS),Windows_NT)
	CFLAGS +=  -I src/libwebp -I src/libwebp/src
endif

FAN_OBJECT_FOLDER = 

all: fan_window.o fan_window_input.o fan_shared_gui.o fan_shared_graphics.o run

LIBS = fan_window.o fan_window_input.o fan_shared_gui.o fan_shared_graphics.o

fan_window.o:  src/fan/window/window.cpp
	$(GPP) $(CFLAGS) -c src/fan/window/window.cpp -o $(FAN_OBJECT_FOLDER)fan_window.o
	
fan_window_input.o:	src/fan/window/window_input.cpp
	$(GPP) $(CFLAGS) -c src/fan/window/window_input.cpp -o $(FAN_OBJECT_FOLDER)fan_window_input.o

fan_shared_gui.o:	src/fan/graphics/shared_gui.cpp
	$(GPP) $(CFLAGS) -c src/fan/graphics/shared_gui.cpp -o $(FAN_OBJECT_FOLDER)fan_shared_gui.o

fan_shared_graphics.o:	src/fan/graphics/shared_graphics.cpp
	$(GPP) $(CFLAGS) -c src/fan/graphics/shared_graphics.cpp -o fan_shared_graphics.o	
clean:
	$(RM) fan_*.o

run:	$(LIBS)
	$(AR) rvs $(LIBNAME) $(LIBS)