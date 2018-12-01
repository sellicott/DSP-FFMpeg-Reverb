
#ifndef _ALLPASS_FILTER_H_
#define _ALLPASS_FILTER_H_

#include <memory>
#include <deque>

using std::size_t;

using std::array;


class AllpassFilter {
using buffPtr = std::unique_ptr<std::deque<double>>;

public:
  AllpassFilter(size_t delay, float gain);
  ~AllpassFilter() = default;

  double do_filtering(double new_x);

private:

  // multiply by 2 to make room for the L/R audio channels
  size_t delay;
  float gain;
  buffPtr input_samples;
  buffPtr output_samples;
};

#endif // _REVERB_UNIT_H_