all: things

Things.o: Things.cc Things.h
	g++ -g -c $<

Socket.o: Socket.cc Socket.h
	g++ -g -c $<

main.o: main.cc Things.h
	g++ -g -c $<



things: main.o Things.o Socket.o cJSON.o
	g++ -g -o $@ main.o Things.o Socket.o cJSON.o -lpthread -lm -lrt


clean:
	rm -f *.o things
