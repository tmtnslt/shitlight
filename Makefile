all : libshitlight

libshitlight : chitlight-api.o minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ -shared -Wl,-soname,libshitlight.so -o libshitlight.so chitlight-api.o minwiringPi/minwiringPi.o -lm -lpthread -lasound -lsamplerate -DUSE_FFTW -lfftw3

chitlight-api.o : chitlight-api.c
	g++ -c -fPIC chitlight-api.c -o chitlight-api.o -I minwiringPi/ -I BTrack/

minwiringPi/minwiringPi.o : minwiringPi/minwiringPi.c
	gcc -c -fPIC minwiringPi/minwiringPi.c -o minwiringPi/minwiringPi.o

BTrack/BTrack.o : BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp
	g++ -c -fPIC BTrack/OnsetDetectionFunction.cpp -o BTrack/OnsetDetectionFunction.o -DUSE_FFTW
	g++ -c -fPIC BTrack/BTrack.cpp -o BTrack/BTrack.o -DUSE_FFTW

demo : chitlight-api.o minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ -o chitlightdemo chitlight-api.o minwiringPi/minwiringPi.o -lm -lpthread -lasound -lsamplerate -DUSE_FFTW -lfftw3

debug : minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ -shared -Wl,-soname,libshitlight.so -o libshitlight.so -fPIC -D_DEBUG -I minwiringPi/ chitlight-api.c minwiringPi/minwiringPi.o -lm -lpthread -lasound -lsamplerate -DUSE_FFTW -lfftw3
