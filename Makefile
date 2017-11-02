.PHONY: clean test

all: test_bin

tinycompo.hpp:
	curl https://raw.githubusercontent.com/vlanore/tinycompo/master/tinycompo.hpp > tinycompo.hpp

test_bin: test.cpp tinycompo.hpp
	$(CXX) test.cpp -o test_bin -lsfml-graphics -lsfml-window -lsfml-system --std=gnu++11

format:
	clang-format -i test.cpp

test: test_bin
	./test_bin

ready: format
	git status

clean:
	rm -f *.o *_bin tinycompo.hpp
