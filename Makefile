#snippets := \
	ffmpeg_decode \
	ffmpeg_play \
	ffmpeg_play_encoder \
	ffmpeg_reverb 
snippets := ffmpeg_reverb
#	decode_audio

all: $(snippets)

clean:
	rm -f $(snippets)

ffmpeg_decode: ffmpeg_decode.cpp Makefile
	g++ -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavutil -lswresample

ffmpeg_play: ffmpeg_play.cpp Makefile
	g++ -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavdevice -lavutil

ffmpeg_play_encoder: ffmpeg_play_encoder.cpp Makefile
	g++ -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavdevice -lavutil

ffmpeg_reverb: ffmpeg_reverb.cpp ReverbUnit.cpp AllpassFilter.cpp FIRFilter.cpp Makefile
	g++ -ggdb -O2 -o $@ $@.cpp ReverbUnit.cpp AllpassFilter.cpp FIRFilter.cpp -lavformat -lavcodec -lavdevice -lavutil -lswresample

#decode_audio: decode_audio.cpp Makefile
#	gcc -ggdb -o $@ $@.cpp -lavformat -lavcodec -lavutil -lswresample