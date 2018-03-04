# FLAGS = -Wall -Wextra -g -fno-inline-functions -O0
FLAGS = -Wall -Wextra

.PHONY: clean test game

all: game_bin

src/tinycompo.hpp:
	curl https://raw.githubusercontent.com/vlanore/tinycompo/master/tinycompo.hpp > $@

%_bin: src/%.cpp src/tinycompo.hpp src/*.hpp
	$(CXX) $< -o $@ $(FLAGS) -lsfml-graphics -lsfml-window -lsfml-system --std=gnu++14

format:
	clang-format -i src/*.cpp src/*.hpp test/*.hpp test/*.cpp

game: game_bin
	./$<

test_bin: test/main.cpp src/*.hpp test/doctest.h src/tinycompo.hpp test/*.hpp
	$(CXX) $< -o test_bin $(FLAGS) -lsfml-graphics -lsfml-window -lsfml-system --std=gnu++14

test: test_bin
	./$<

ready: all format test
	git status

clean:
	rm -f *.o *_bin src/tinycompo.hpp tmp*
