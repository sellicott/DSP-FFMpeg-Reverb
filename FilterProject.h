#ifndef _FILTER_PROJECT_H
#define _FILTER_PROJECT_H

#include <cstdint>
#include <memory>
#include <deque>
#include "AllpassFilter.h"
#include "FIRFilter.h"

using std::size_t;
using std::uint8_t;
using std::int16_t;
using std::array;


class ReverbUnit {
using outType = int32_t;
using deque = std::deque<outType>;
using buffPtr = std::unique_ptr<deque>;

public:
  ReverbUnit(float feedbackGain_ = 0.25);
  ~ReverbUnit() = default;

  // the decoder and playback programs need the take the samples in the form of 8-bit integers
  uint8_t* get_samples(uint8_t* samples, size_t num_samples);

private:

  float reverb_gain;
  AllpassFilter allpass1;
  AllpassFilter allpass2;
  AllpassFilter allpass3;
  AllpassFilter allpass4;
  FIRFilter firFilter;
  buffPtr delay;
  outType do_filtering(outType new_x);

  float feedbackGain;
  const size_t sample_rate = 44100;
};

#endif //_FILTER_PROJECT_H