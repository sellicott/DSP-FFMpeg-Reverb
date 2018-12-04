
#ifndef _ALLPASS_FILTER_H_
#define _ALLPASS_FILTER_H_

#include <memory>
#include <deque>

using std::size_t;

using std::array;


class AllpassFilter {
using outType = float;
using deque = std::deque<outType>;
using buffPtr = std::unique_ptr<deque>;

public:
  AllpassFilter(size_t delay, float gain);
  ~AllpassFilter() = default;

  outType do_filtering(outType new_x);

private:

  // multiply by 2 to make room for the L/R audio channels
  size_t delay;
  float gain;
  buffPtr delay_buff;
};

#endif // _REVERB_UNIT_H_