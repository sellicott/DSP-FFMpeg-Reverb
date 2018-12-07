#ifndef _REVERB_UNIT_H_
#define _REVERB_UNIT_H_

#include <cstdint>
#include <memory>
#include <deque>
#include "AllpassFilter.h"
#include "FIRFilter.h"
#include "CombFilter.h"

using std::size_t;
using std::uint8_t;
using std::int16_t;
using std::array;


class ReverbUnit {
using outType = int32_t;
using deque = std::deque<outType>;
using buffPtr = std::unique_ptr<deque>;

public:
  ReverbUnit();
  ~ReverbUnit() = default;

  uint8_t* get_samples(uint8_t* samples, size_t num_samples);

private:

  float reverb_gain;
  AllpassFilter filter1;
  AllpassFilter filter2;
  AllpassFilter filter3;
  AllpassFilter filter4;
  FIRFilter firFilter;
  buffPtr delay;
  outType do_filtering(outType new_x);

  const size_t sample_rate = 44100;
};

#endif //_REVERB_UNIT_H_