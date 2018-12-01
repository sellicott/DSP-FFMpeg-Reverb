#ifndef _REVERB_UNIT_H_
#define _REVERB_UNIT_H_

#include <cstdint>
#include <memory>
#include "CircularBuffer.h"

using std::size_t;
using std::uint8_t;

using std::array;
using buffPtr = std::unique_ptr<CircularBuffer<float>>;


class ReverbUnit {

public:
  ReverbUnit(size_t outbuff_size_ = 512);
  ~ReverbUnit() = default;

  uint8_t* get_samples(uint8_t* samples, size_t num_samples);

private:
  float do_filtering(float new_x);

  // multiply by 2 to make room for the L/R audio channels
  const size_t sample_rate = 44100*2;
  buffPtr input_samples;
  buffPtr output_samples;
  size_t outbuff_size;
};

#endif //_REVERB_UNIT_H_