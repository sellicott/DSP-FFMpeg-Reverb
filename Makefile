snippets := \
	ffmpeg_decode \
	ffmpeg_play \
	ffmpeg_play_encoder 

all: $(snippets)

clean:
	rm -f $(snippets)

ffmpeg_decode: ffmpeg_decode.cpp Makefile
	g++ -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavutil -lswresample

ffmpeg_play: ffmpeg_play.cpp Makefile
	g++ -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavdevice -lavutil

ffmpeg_play_encoder: ffmpeg_play_encoder.cpp Makefile
	g++ -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavdevice -lavutil