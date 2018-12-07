/* Perform filtering on audio samples 
 * - reads input from stdin and writes filtered samples to stdout
 * - two channels (front left, front right)
 * - samples in interleaved format (L R L R ...)
 * - samples are 16-bit signed integers (what the Rpi needs)
 *
 * Usage:
 *   ./ffmpeg_decode cool_song.mp3 | ./filter | ./ffmpeg_play 
 */

#include "FilterProject.h"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <memory>

using std::string;
using std::array;
using std::uint8_t;
using std::int16_t;


ReverbUnit::ReverbUnit(float feedbackGain_) : 
  reverb_gain(0.6),
  //initilize filters
  allpass1(4410, 0.6), 
  allpass2(1470, -0.6), 
  allpass3(490, 0.6),
  allpass4(163, -0.6),

  // pass in FIR coefficients to the FIR filter class
  firFilter({ 0.003369,0.002810,0.001758,0.000340,-0.001255,-0.002793,-0.004014,
    -0.004659,-0.004516,-0.003464,-0.001514,0.001148,0.004157,0.006986,0.009003,
    0.009571,0.008173,0.004560,-0.001120,-0.008222,-0.015581,-0.021579,-0.024323,
    -0.021933,-0.012904,0.003500,0.026890,0.055537,0.086377,0.115331,0.137960,
    0.150407,0.150407,0.137960,0.115331,0.086377,0.055537,0.026890,0.003500,
    -0.012904,-0.021933,-0.024323,-0.021579,-0.015581,-0.008222,-0.001120,
    0.004560,0.008173,0.009571,0.009003,0.006986,0.004157,0.001148,-0.001514,
    -0.003464,-0.004516,-0.004659,-0.004014,-0.002793,-0.001255,0.000340,
    0.001758,0.002810,0.003369 })
{
  delay = std::make_unique<deque>(3*2*2940, 0.0);

  feedbackGain = 0.25;
  if (feedbackGain_ > 0 && feedbackGain_ < 1) {
    feedbackGain = feedbackGain_;
  }
}

// function to run on the samples from stdin
uint8_t* ReverbUnit::get_samples(uint8_t* samples, size_t num_samples) {

  //convert the uint8_t samples to floating point
  auto samples_cast = reinterpret_cast<int16_t*>(samples);
  auto num_samples_cast = num_samples/sizeof(int16_t);

  for (auto i = 0; i < num_samples_cast; i++) {
    //filters on!
    auto left_sample = samples_cast[i];
    auto filtered = reverb_gain*do_filtering(left_sample);
    samples_cast[i] = filtered + left_sample;
    ++i;
    auto right_sample = samples_cast[i];
    filtered = reverb_gain*do_filtering(right_sample);
    samples_cast[i] = filtered + right_sample;
  }

  return reinterpret_cast<uint8_t*>(samples);  
}

ReverbUnit::outType ReverbUnit::do_filtering(outType new_x) {
  auto &d = *delay.get();

  // the coefficient on the d.back() sets how long the reverb 
  // will sustain: larger = longer 
  auto x = 0.7*new_x + feedbackGain*d.back();

  //run through the all pass filters
  auto temp = allpass1.do_filtering(x);
  temp = allpass2.do_filtering(temp);
  temp = allpass3.do_filtering(temp);
  temp = allpass4.do_filtering(temp);
  temp = firFilter.do_filtering(temp);

  d.pop_back();
  d.push_front(temp);

  //add a bit of an FIR filter here, smooth the output
  auto y = temp + 0.5*d[2*2940] + 0.25*d[2*2*2940] + 0.125*d.back();
  return y;
}


// ---------------------------------------------main --------------------------------------------------
int main(int argc, char** argv) {
  if (argc != 1) {
      fprintf(stderr, "usage: ./ffmpeg_decode <input file> | %s | ./ffmpeg_play\n", argv[0]);
      exit(1);
  }

  //some constants
  const int BUFF_SIZE = 4096;
  array<uint8_t, BUFF_SIZE> buffer;
  ReverbUnit reverb(0.25);

  for (;;) {

    // read input buffer from stdin
    ssize_t ret = read(STDIN_FILENO, buffer.data(), buffer.size());
    if (ret < 0) {
        fprintf(stderr, "read(stdin)\n");
        exit(1);
    }

    //exit if out of data
    if (ret == 0) {
        break;
    }

    // do the filtering
    reverb.get_samples(buffer.data(), buffer.size());

    // write output buffer to stdout
    if (write(STDOUT_FILENO, buffer.data(), buffer.size()) != buffer.size()) {
      fprintf(stderr, "error: write(stdout)\n");
      exit(1);
    }
  }

  return 0;
}