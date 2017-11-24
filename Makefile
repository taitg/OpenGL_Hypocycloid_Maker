EXEFILE = cpsc589a1
LIBS = -lglfw -lGL -lGLU


$(EXEFILE): main.o
	g++ $(LIBS) -o $(EXEFILE) main.o

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm $(EXEFILE) main.o
