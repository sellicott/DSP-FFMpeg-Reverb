/* Perform filtering on audio samples 
 * - reads input from stdin and writes filtered samples to stdout
 * - two channels (front left, front right)
 * - samples in interleaved format (L R L R ...)
 * - samples are 16-bit signed integers (what the Rpi needs)
 *
 * Usage:
 *   ./ffmpeg_decode cool_song.mp3 | ./filter | ./ffmpeg_play 
 */

#include "Template.h"

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

// Constructor implementation
FilterProject::FilterProject() : 
  //initilize filters
  allpassFilter(4410, 0.6), 

  //add your comma separated filters to initilize here 

  // pass in FIR coefficients to the FIR filter class
  firFilter({ 0.5, 0.7, 0.5 /* Put your comma separated filter coefficients here */})

  // note the last filter does not have a comma after it
{ }

FilterProject::outType FilterProject::do_filtering(outType new_x) {

  // run through the all pass filters
  auto allpass_out = allpassFilter.do_filtering(new_x);
  auto FIR_out = firFilter.do_filtering(new_x);

  // we want to use the allpass filter output
  auto y = allpass_out;
  return y;
}

// function to run on the samples from stdin
uint8_t* FilterProject::get_samples(uint8_t* samples, size_t num_samples) {

  //convert the uint8_t samples to floating point
  auto samples_cast = reinterpret_cast<int16_t*>(samples);
  auto num_samples_cast = num_samples/sizeof(int16_t);

  for (auto i = 0; i < num_samples_cast; i++) {
    //filters on!
    auto left_sample = samples_cast[i];
    samples_cast[i] = do_filtering(left_sample);
    ++i;
    auto right_sample = samples_cast[i];
    samples_cast[i] = do_filtering(right_sample);
  }

  return reinterpret_cast<uint8_t*>(samples);  
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
  FilterProject filter;

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
    filter.get_samples(buffer.data(), buffer.size());

    // write output buffer to stdout
    if (write(STDOUT_FILENO, buffer.data(), buffer.size()) != buffer.size()) {
      fprintf(stderr, "error: write(stdout)\n");
      exit(1);
    }
  }

  return 0;
}