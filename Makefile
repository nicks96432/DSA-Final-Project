all:
	g++ main.cpp -o run -O2 -std=c++14
clean:
	rm -rf run
run:
	run
debug_run:
	g++ main.cpp -o run -O2 -std=c++14 -DDEBUG
	run < 2.in