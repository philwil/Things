all: things t2 libt2gpio.so
CPP=g++

Things.o: Things.cc Things.h
	$(CPP) -g -c $<

Socket.o: Socket.cc Socket.h 
	$(CPP)	 -g -c $< -fPIC

main.o: main.cc Things.h
	$(CPP) -g -c $<

t2_main.o: t2_main.cc T2.h
	$(CPP) -g -c $<

T2.o: T2.cc T2.h
	$(CPP) -g -c -fPIC $<

libt2gpio.so: t2gpio.cc T2.o Socket.o
	$(CPP) -o $@ $< T2.o Socket.o -shared -fPIC

things: main.o Things.o Socket.o cJSON.o
	$(CPP) -g -o $@ main.o Things.o Socket.o cJSON.o -lpthread -lm -lrt

t2:    t2_main.o T2.o
	$(CPP) -g -o $@ t2_main.o T2.o Socket.o cJSON.o -lpthread -lm -lrt -ldl

clean:
	rm -f *.o *.so things t2
