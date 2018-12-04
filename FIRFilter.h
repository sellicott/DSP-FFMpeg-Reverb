
#ifndef _FIR_FILTER_H_
#define _FIR_FILTER_H_

#include <memory>
#include <deque>
#include <vector>

using std::size_t;
using std::int32_t;

using std::array;


class FIRFilter {
using outType = int32_t;
using deque = std::deque<outType>;
using buffPtr = std::unique_ptr<deque>;

public:
  FIRFilter(std::vector<float> taps_);
  ~FIRFilter() = default;

  outType do_filtering(outType new_x);

private:

  size_t delay;
  std::vector<float> taps;
  buffPtr input_samples;
};

#endif // _REVERB_UNIT_H_