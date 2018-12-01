
#ifndef _COMB_FILTER_H_
#define _COMB_FILTER_H_

#include <memory>
#include "CircularBuffer.h"
#include <deque>

using std::size_t;

using std::array;


class CombFilter {
//using buffPtr = std::unique_ptr<CircularBuffer<double>>;
using buffPtr = std::unique_ptr<std::deque<double>>;

public:
  CombFilter(size_t delay, float gain);
  ~CombFilter() = default;

  double do_filtering(double new_x);

private:

  // multiply by 2 to make room for the L/R audio channels
  size_t delay;
  float gain;
  buffPtr input_samples;
  buffPtr output_samples;
};

#endif // _REVERB_UNIT_H_