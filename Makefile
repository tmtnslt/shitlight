all : libshitlight

libshitlight : chitlight-api.o minwiringPi/minwiringPi.o
	gcc -shared -Wl,-soname,libshitlight.so -o libshitlight.so chitlight-api.o minwiringPi/minwiringPi.o -lm -lpthread

chitlight-api.o : chitlight-api.c
	gcc -c -fPIC chitlight-api.c -o chitlight-api.o -I minwiringPi/

minwiringPi/minwiringPi.o : minwiringPi/minwiringPi.c
	gcc -c -fPIC minwiringPi/minwiringPi.c -o minwiringPi/minwiringPi.o

demo : chitlight-api.o minwiringPi/minwiringPi.o
	gcc -o chitlightdemo chitlight-api.o minwiringPi/minwiringPi.o -lm -lpthread

debug : minwiringPi/minwiringPi.o
	gcc -shared -Wl,-soname,libshitlight.so -o libshitlight.so -fPIC -D_DEBUG -I minwiringPi/ chitlight-api.c minwiringPi/minwiringPi.o -lm -lpthread
