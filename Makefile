all : libshitlight

libshitlight : chitlight-api.o minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ --std=c++11 -shared -Wl,-soname,libshitlight.so -o libshitlight.so chitlight-api.c minwiringPi/minwiringPi.c BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp -I minwiringPi/ -I BTrack/ -lm -lasound -lsamplerate -DUSE_FFTW -lfftw3 -fPIC -pthread

chitlight-api.o : chitlight-api.c
	g++ --std=c++11 -c -fPIC chitlight-api.c -o chitlight-api.o -I minwiringPi/ -I BTrack/

minwiringPi/minwiringPi.o : minwiringPi/minwiringPi.c
	g++ -c -fPIC minwiringPi/minwiringPi.c -o minwiringPi/minwiringPi.o

BTrack/BTrack.o : BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp
	g++ --std=c++11 -c -fPIC BTrack/OnsetDetectionFunction.cpp -o BTrack/OnsetDetectionFunction.o -DUSE_FFTW
	g++ --std=c++11 -c -fPIC BTrack/BTrack.cpp -o BTrack/BTrack.o -DUSE_FFTW

demo : chitlight-api.o minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ --std=c++11 -o chitlightdemo chitlight-api.c minwiringPi/minwiringPi.c BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp -I minwiringPi/ -I BTrack/ -lm -lasound -lsamplerate -DUSE_FFTW -lfftw3 -fpermissive -pthread

debugdemo : chitlight-api.o minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ --std=c++11 -o chitlightdemo chitlight-api.c minwiringPi/minwiringPi.c BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp -I minwiringPi/ -I BTrack/ -lm -lasound -lsamplerate -DUSE_FFTW -lfftw3 -D_DEBUGBPM -pthread

debug : minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ --std=c++11 -shared -Wl,-soname,libshitlight.so -o libshitlight.so chitlight-api.c minwiringPi/minwiringPi.c BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp -I minwiringPi/ -I BTrack/ -lm -lasound -lsamplerate -DUSE_FFTW -lfftw3 -D_DEBUG -fPIC -pthread
debugbpm : minwiringPi/minwiringPi.o BTrack/BTrack.o
	g++ --std=c++11 -shared -Wl,-soname,libshitlight.so -o libshitlight.so chitlight-api.c minwiringPi/minwiringPi.c BTrack/BTrack.cpp BTrack/OnsetDetectionFunction.cpp -I minwiringPi/ -I BTrack/ -lm -lasound -lsamplerate -DUSE_FFTW -lfftw3 -D_DEBUGBPM -fPIC -pthread
