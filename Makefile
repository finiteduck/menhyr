.PHONY: clean test

all: test_bin rework_bin

tinycompo.hpp:
	curl https://raw.githubusercontent.com/vlanore/tinycompo/master/tinycompo.hpp > tinycompo.hpp

%_bin: %.cpp tinycompo.hpp
	$(CXX) $< -o $@ -lsfml-graphics -lsfml-window -lsfml-system --std=gnu++11

format:
	clang-format -i test.cpp rework.cpp

test: test_bin
	./$<

rework: rework_bin
	./$<

ready: all format
	git status

clean:
	rm -f *.o *_bin tinycompo.hpp
