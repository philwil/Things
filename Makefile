all: things

Things.o: Things.cc Things.h
	g++ -g -c $<

socket.o: socket.cc
	g++ -g -c $<

main.o: main.cc Things.h
	g++ -g -c $<



things: main.o Things.o socket.o cJSON.o
	g++ -g -o $@ main.o Things.o socket.o cJSON.o -lpthread -lm -lrt


clean:
	rm -f *.o things
