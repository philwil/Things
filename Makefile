all: things t2 libt2gpios.so libt2tcps.so Strings
CPP=g++

Things.o: Things.cc Things.h
	$(CPP) -g -c $<

Socket.o: Socket.cc Socket.h 
	$(CPP)	 -g -c $< -fPIC

Strings.o: Strings.cc Strings.h 
	$(CPP)	 -g -c $< -fPIC

main.o: main.cc Things.h
	$(CPP) -g -c $<

Strings: Strings.cc Strings.h
	$(CPP) -g -o $@ -DTEST_MAIN  $<

t2_main.o: t2_main.cc T2.h
	$(CPP) -g -c $<

T2.o: T2.cc T2.h
	$(CPP) -g -c -fPIC $<

libt2gpios.so: t2gpios.cc T2.o Strings.o
	$(CPP) -o $@ $< T2.o  Strings.o -shared -fPIC

libt2tcps.so: t2tcps.cc T2.o Strings.o 
	$(CPP) -o $@ $< T2.o Strings.o  -shared -fPIC

things: main.o Things.o Socket.o cJSON.o Strings.o
	$(CPP) -g -o $@ main.o Things.o Strings.o Socket.o cJSON.o -lpthread -lm -lrt

t2:    t2_main.o T2.o Strings.o cJSON.o
	$(CPP) -g -o $@ t2_main.o T2.o Strings.o cJSON.o -lpthread -lm -lrt -ldl

clean:
	rm -f *.o *.so things t2 Strings
