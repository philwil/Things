all: things t2


Things.o: Things.cc Things.h
	g++ -g -c $<

Socket.o: Socket.cc Socket.h
	g++ -g -c $<

main.o: main.cc Things.h
	g++ -g -c $<

t2_main.o: t2_main.cc T2.h
	g++ -g -c $<

T2.o: T2.cc T2.h
	g++ -g -c $<



things: main.o Things.o Socket.o cJSON.o
	g++ -g -o $@ main.o Things.o Socket.o cJSON.o -lpthread -lm -lrt

t2:    t2_main.o T2.o
	g++ -g -o $@ t2_main.o T2.o Socket.o cJSON.o -lpthread -lm -lrt

clean:
	rm -f *.o things t2
