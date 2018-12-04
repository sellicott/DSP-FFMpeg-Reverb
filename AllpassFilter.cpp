#include "AllpassFilter.h"

AllpassFilter::AllpassFilter(size_t delay_, float gain_) {

  // multiply by 2 to make room for the L/R audio channels
  delay = delay_ * 2;
  gain = gain_;
  // initilize the buffer to zeros
  delay_buff = std::make_unique<deque>(delay, 0.0);
}

AllpassFilter::outType AllpassFilter::do_filtering(outType new_x) {
  // grab the delay line
  auto &g = *delay_buff.get();

  //get the latest value to come through the delay line
  auto g_out = g.back();
  g.pop_back();

  // do the thing! follows the standard allpass filter structure
  auto g_in = new_x + gain*g_out;
  g.push_front(g_in);
  auto y = -gain*g_in + g_out;

  //return the newest value for y
  return y;
}