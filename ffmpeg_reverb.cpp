/* Decode audio file using ffmpeg and write decoded samples to stdout.
 * Output format:
 *  - two channels (front left, front right)
 *  - samples in interleaved format (L R L R ...)
 *  - samples are 32-bit floats
 *  - sample rate is 44100
 *
 * Usage:
 *   ./ffmpeg_decode cool_song.mp3 > cool_song_samples
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <memory>
#include "ReverbUnit.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavdevice/avdevice.h>
}

using std::string;

typedef struct ffmpeg_decode_init_t {
  AVFormatContext* fmt_ctx;
  AVCodecContext*  codec_ctx;
  AVCodec*         codec;
  SwrContext*      swr_ctx;
  int max_buffer_size;
  size_t stream_idx;
} ffmpeg_init_t;

typedef struct ffmpeg_play_init_t {
  AVFormatContext* fmt_ctx;
  AVStream* stream;
  AVCodecContext* codec_ctx;
  int max_buffer_size;
} ffmpeg_play_t;

// helper functions
ffmpeg_decode_init_t init_ffmpeg_decoder(string filename, int out_samples = 512, int sample_rate = 44100); 
ffmpeg_play_t init_ffmpeg_play(int in_channels = 2, int in_samples = 512, int sample_rate = 44100, int bitrate = 64000);



void deinit_ffmpeg_decoder(ffmpeg_decode_init_t init_struct);
void deinit_ffmpeg_play(ffmpeg_play_init_t init_struct);

ffmpeg_decode_init_t init_ffmpeg_decoder(string filename, int out_samples, int sample_rate){

  ffmpeg_decode_init_t init_struct = {0};

  const int out_channels = 2;

  init_struct.max_buffer_size =
      av_samples_get_buffer_size(
          NULL, out_channels, out_samples, AV_SAMPLE_FMT_FLT, 1);


  // allocate empty format context
  // provides methods for reading input packets
  init_struct.fmt_ctx = avformat_alloc_context();
  assert(init_struct.fmt_ctx);

  // determine input file type and initialize format context
  if (avformat_open_input(&init_struct.fmt_ctx, filename.c_str(), NULL, NULL) != 0) {
      fprintf(stderr, "error: avformat_open_input()\n");
      exit(1);
  }

  // determine supported codecs for input file streams and add
  // them to format context
  if (avformat_find_stream_info(init_struct.fmt_ctx, NULL) < 0) {
      fprintf(stderr, "error: avformat_find_stream_info()\n");
      exit(1);
  }

  // find audio stream in format context
  size_t stream = 0;
  for (; stream < init_struct.fmt_ctx->nb_streams; stream++) {
      if (init_struct.fmt_ctx->streams[stream]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
          break;
      }
  }
  if (stream == init_struct.fmt_ctx->nb_streams) {
      fprintf(stderr, "error: no audio stream found\n");
      exit(1);
  }

  //av_dump_format(init_struct.fmt_ctx, 0, filename.c_str(), 0);
  init_struct.stream_idx = stream;

  // get codec context for audio stream
  // provides methods for decoding input packets received from format context
  init_struct.codec_ctx = init_struct.fmt_ctx->streams[stream]->codec;
  assert(init_struct.codec_ctx);

  if (init_struct.codec_ctx->channel_layout == 0) {
      init_struct.codec_ctx->channel_layout = AV_CH_FRONT_LEFT | AV_CH_FRONT_RIGHT;
  }

  // find decoder for audio stream
  init_struct.codec = avcodec_find_decoder(init_struct.codec_ctx->codec_id);
  if (!init_struct.codec) {
      fprintf(stderr, "error: avcodec_find_decoder()\n");
      exit(1);
  }

  // initialize codec context with decoder we've found
  if (avcodec_open2(init_struct.codec_ctx, init_struct.codec, NULL) < 0) {
      fprintf(stderr, "error: avcodec_open2()\n");
      exit(1);
  }

  // initialize converter from input audio stream to output stream
  // provides methods for converting decoded packets to output stream
  init_struct.swr_ctx  =
      swr_alloc_set_opts(NULL,
                          AV_CH_FRONT_LEFT | AV_CH_FRONT_RIGHT, // output
                          AV_SAMPLE_FMT_FLT,                    // output
                          sample_rate,                          // output
                          init_struct.codec_ctx->channel_layout,  // input
                          init_struct.codec_ctx->sample_fmt,      // input
                          init_struct.codec_ctx->sample_rate,     // input
                          0,
                          NULL);
  if (!init_struct.swr_ctx) {
      fprintf(stderr, "error: swr_alloc_set_opts()\n");
      exit(1);
  }
  swr_init(init_struct.swr_ctx);

  return init_struct;
}

ffmpeg_play_init_t init_ffmpeg_play(int in_channels, int in_samples, int sample_rate, int bitrate) {

  ffmpeg_play_init_t init_struct = {0};

  init_struct.max_buffer_size =
      av_samples_get_buffer_size(
          NULL, in_channels, in_samples, AV_SAMPLE_FMT_FLT, 1);

  AVOutputFormat* fmt = av_guess_format("alsa", NULL, NULL);
  if (!fmt) {
      fprintf(stderr, "av_guess_format()\n");
      exit(1);
  }

  // allocate empty format context
  // provides methods for writing output packets
  init_struct.fmt_ctx = avformat_alloc_context();
  if (!init_struct.fmt_ctx) {
      fprintf(stderr, "avformat_alloc_context()\n");
      exit(1);
  }

  // tell format context to use ALSA as ouput device
  init_struct.fmt_ctx->oformat = fmt;

  // add stream to format context
  init_struct.stream = avformat_new_stream(init_struct.fmt_ctx, NULL);
  if (!init_struct.stream) {
      fprintf(stderr, "avformat_new_stream()\n");
      exit(1);
  }

  // initialize stream codec context
  // format conetxt uses codec context when writing packets
  init_struct.codec_ctx = init_struct.stream->codec;
  assert(init_struct.codec_ctx);
  init_struct.codec_ctx->codec_id = AV_CODEC_ID_PCM_F32LE;
  init_struct.codec_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
  init_struct.codec_ctx->sample_fmt = AV_SAMPLE_FMT_FLT;
  init_struct.codec_ctx->bit_rate = bitrate;
  init_struct.codec_ctx->sample_rate = sample_rate;
  init_struct.codec_ctx->channels = in_channels;
  init_struct.codec_ctx->channel_layout = AV_CH_FRONT_LEFT | AV_CH_FRONT_RIGHT;

  // initialze output device
  if (avformat_write_header(init_struct.fmt_ctx, NULL) < 0) {
      fprintf(stderr, "avformat_write_header()\n");
      exit(1);
  }

  return init_struct;
}

void deinit_ffmpeg_decoder(ffmpeg_decode_init_t init_struct) {
  swr_free(&init_struct.swr_ctx);

  avcodec_close(init_struct.codec_ctx);
  avformat_close_input(&init_struct.fmt_ctx);
}

void deinit_ffmpeg_play(ffmpeg_play_init_t init_struct) {
  avformat_free_context(init_struct.fmt_ctx);
}

// ---------------------------------------------main --------------------------------------------------
int main(int argc, char** argv) {
  if (argc != 1) {
      fprintf(stderr, "usage: %s < input_file\n", argv[0]);
      exit(1);
  }

  const int out_channels = 2, out_samples = 512, sample_rate = 44100;
  ReverbUnit reverb(out_samples);

  // register supported formats and codecs
  av_register_all();

  // register supported devices
  avdevice_register_all();

  //initilize ffmpeg
  auto play_struct = init_ffmpeg_play();
  printf("everything initilized\n");

  // allocate buffer for input samples
  uint8_t* buffer = (uint8_t*)av_malloc(play_struct.max_buffer_size);
  assert(buffer);

  // initialze output device
  if (avformat_write_header(play_struct.fmt_ctx, NULL) < 0) {
      fprintf(stderr, "avformat_write_header()\n");
      exit(1);
  }

  for (;;) {
      memset(buffer, 0, play_struct.max_buffer_size);

      // read input buffer from stdin
      ssize_t ret = read(STDIN_FILENO, buffer, play_struct.max_buffer_size);
      if (ret < 0) {
          fprintf(stderr, "read(stdin)\n");
          exit(1);
      }

      if (ret == 0) {
          break;
      }

      // create output packet
      AVPacket packet;
      av_init_packet(&packet);
      packet.data = reverb.get_samples(buffer, play_struct.max_buffer_size);
      packet.size = play_struct.max_buffer_size;

      // write output packet to format context
      if (av_write_frame(play_struct.fmt_ctx, &packet) < 0) {
          fprintf(stderr, "av_write_frame()\n");
          exit(1);
      }
  }

  av_free(buffer);
  deinit_ffmpeg_play(play_struct);

  return 0;
}