all: main.exe

run: main.exe
	./main.exe

main.exe: main.o
	g++ -o main.exe main.o

main.o: main.cc
	g++ -c main.cc

clean:
	del *.o
	del *.exe

