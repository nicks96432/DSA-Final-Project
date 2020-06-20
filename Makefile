all:
	g++ main.cpp -o run -O2 -std=c++11
clean:
	rm -rf run
run:
	run
debug_run:
	/usr/bin/time -v ./run