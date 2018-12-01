
#ifndef _FIR_FILTER_H_
#define _FIR_FILTER_H_

#include <memory>
#include <deque>
#include <vector>

using std::size_t;

using std::array;


class FIRFilter {
using buffPtr = std::unique_ptr<std::deque<double>>;

public:
  FIRFilter(std::vector<float> taps_);
  ~FIRFilter() = default;

  double do_filtering(double new_x);

private:

  // multiply by 2 to make room for the L/R audio channels
  size_t delay;
  std::vector<float> taps;
  buffPtr input_samples;
  buffPtr output_samples;
};

#endif // _REVERB_UNIT_H_