.PHONY: clean test

all: test_bin

test_bin: test.cpp
	$(CXX) test.cpp -o test_bin -lsfml-graphics -lsfml-window -lsfml-system

test: test_bin
	./test_bin

clean:
	rm -f *.o *_bin
