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


class FilterProject{
private:
  using outType = int16_t;
  using deque = std::deque<outType>;
  using buffPtr = std::unique_ptr<deque>;

public:
  //constructor
  FilterProject();

  // the decoder and playback programs need the take the samples in the form of 8-bit integers
  uint8_t* get_samples(uint8_t* samples, size_t num_samples);

private:
  //define the names of your filters here 
  AllpassFilter allpassFilter;
  FIRFilter firFilter;

  outType do_filtering(outType new_x);

  const size_t sample_rate = 44100;
};

#endif //_FILTER_PROJECT_H