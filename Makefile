all:
	g++ main.cpp -o run -O2 -std=c++14
clean:
	rm -rf run
run:
	run
