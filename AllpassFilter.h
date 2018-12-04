
#ifndef _ALLPASS_FILTER_H_
#define _ALLPASS_FILTER_H_

#include <memory>
#include <deque>

using std::size_t;
using std::int32_t;

using std::array;


class AllpassFilter {
using outType = int32_t;
using deque = std::deque<outType>;
using buffPtr = std::unique_ptr<deque>;

public:
  AllpassFilter(size_t delay, float gain);
  ~AllpassFilter() = default;

  outType do_filtering(outType new_x);

private:
  size_t delay;
  float gain;
  buffPtr delay_buff;
};

#endif // _REVERB_UNIT_H_