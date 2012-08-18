SOURCE = $(wildcard src/*.cc)
OBJECTS = $(patsubst src/%.cc,obj/%.o,$(SOURCE))
#obj/board.o obj/controller.o obj/cross.o obj/naught.o obj/square.o obj/game.o obj/main.o obj/piece.o
HEADERS = $(wildcard src/*.h)

obj/%.o: src/%.cc
	g++ -g -c `wx-config --cxxflags` -o $@  $^
	
all: main

main: $(OBJECTS) $(HEADERS)
	g++ -g -o $@ $(OBJECTS) -lsqlite3 `wx-config --libs --gl_libs`

clean:
	rm -rf obj/* main
