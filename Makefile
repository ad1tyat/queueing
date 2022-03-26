clean : 
	rm -rf a.out debug.txt output.txt 

run :
	g++ routing.cpp
	./a.out -N 5 -B 4 -p 0.5 -queue ISLIP -T 10000