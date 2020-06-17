all:
	g++ main.cpp -o main -O2 -std=c++11
clean:
	rm -rf main
run:
	./main
debug_run:
	/usr/bin/time -v ./main