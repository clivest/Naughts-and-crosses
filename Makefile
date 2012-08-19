SOURCE = $(wildcard src/*.cc)
OBJECTS = $(patsubst src/%.cc,obj/%.o,$(SOURCE))
HEADERS = $(wildcard src/*.h)
BOOSTINC = /home/clive/Documents/c++/boost_lib/install/include
BOOSTLIB = /home/clive/Documents/c++/boost_lib/install/lib

obj/%.o: src/%.cc
	g++ -g -I$(BOOSTINC) -c `wx-config --cxxflags` -o $@  $^
	
all: main

main: $(OBJECTS) $(HEADERS)
	g++ -g -I$(BOOSTINC) -o $@ $(OBJECTS) -lsqlite3 -L$(BOOSTLIB) -lboost_thread `wx-config --libs --gl_libs`

clean:
	rm -rf obj/* main
