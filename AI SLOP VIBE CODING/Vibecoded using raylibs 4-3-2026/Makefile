MSYS2 = C:/msys64/ucrt64
CXX = $(MSYS2)/bin/g++
INCLUDES = -I$(MSYS2)/include
LIBS = -L$(MSYS2)/lib -static -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -static-libgcc -static-libstdc++

all: main.exe

run: main.exe
	./main.exe

main.exe: main.o
	$(CXX) -o main.exe main.o $(LIBS)

main.o: main.cc
	$(CXX) -c main.cc $(INCLUDES)

clean:
	del *.o
	del *.exe

