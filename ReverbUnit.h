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

using std::array;


class ReverbUnit {
using buffPtr = std::unique_ptr<std::deque<double>>;

public:
  ReverbUnit(size_t outbuff_size_ = 512);
  ~ReverbUnit() = default;

  uint8_t* get_samples(uint8_t* samples, size_t num_samples);

private:

  float reverb_gain;
  AllpassFilter filter1;
  AllpassFilter filter2;
  AllpassFilter filter3;
  AllpassFilter filter4;
  AllpassFilter filter5;
  FIRFilter firFilter1;
  FIRFilter firFilter2;
  CombFilter comb1;
  CombFilter comb2;
  CombFilter comb3;
  CombFilter comb4;
  buffPtr delay1;
  buffPtr delay2;
  buffPtr delay3;
  double do_filtering(double new_x);

  const size_t sample_rate = 44100;
  size_t outbuff_size;
};

#endif //_REVERB_UNIT_H_