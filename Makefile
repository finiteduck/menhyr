.PHONY: clean test

all: test_bin

src/tinycompo.hpp:
	curl https://raw.githubusercontent.com/vlanore/tinycompo/master/tinycompo.hpp > $@

%_bin: src/%.cpp src/tinycompo.hpp src/*.hpp
	$(CXX) $< -o $@ -lsfml-graphics -lsfml-window -lsfml-system --std=gnu++11

format:
	clang-format -i src/*.cpp src/*.hpp

test: test_bin
	./$<

ready: all format
	git status

clean:
	rm -f *.o *_bin src/tinycompo.hpp tmp*
