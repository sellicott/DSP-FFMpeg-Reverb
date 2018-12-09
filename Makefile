SRC :=  FilterProject.cpp AllpassFilter.cpp FIRFilter.cpp

CFLAGS = -O2 -g

.PHONY: clean

SRC_OBJ := $(SRC:.cpp=.o)

.c.o:
	g++ -c $(CFLAGS) $< -o $@

.cpp.o:
	g++ -c $(CFLAGS) $< -o $@

all: $(SRC_OBJ) ffmpeg_decode ffmpeg_play ffmpeg_filter 

ffmpeg_decode: ffmpeg_decode.cpp ffmpeg_decode.o Makefile
	g++ -o $@ $@.o -lavformat -lavcodec -lavutil -lswresample

ffmpeg_play: ffmpeg_play.cpp ffmpeg_play.o Makefile
	g++ -o $@ $@.o -lavformat -lavcodec -lavdevice -lavutil

ffmpeg_filter: $(SRC_OBJ) Makefile
	g++ $(SRC_OBJ) -o ffmpeg_filter 

clean: 
	rm -f $(SRC_OBJ) ./ffmpeg_decode ./ffmpeg_filter ./ffmpeg_play *.o