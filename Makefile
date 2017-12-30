default: build

build:
	g++ -g -Wall -o snake src/main.cc  -lncurses

run: build
	./snake

tests:
	checkmk test/test.ts > test/test.cc
	g++ -o run_tests -lncurses -lcheck
	./run_tests

clean:
	rm snake run_tests
