all: things

Things.o: Things.cc Things.h
	g++ -c $<

socket.o: socket.cc
	g++ -c $<

main.o: main.cc Things.h
	g++ -c $<



things: main.o Things.o socket.o cJSON.o
	g++ -o $@ main.o Things.o socket.o cJSON.o -lpthread -lm -lrt


clean:
	rm -f *.o things
