#include "AllpassFilter.h"

AllpassFilter::AllpassFilter(size_t delay_, float gain_) {

  delay = delay_ * 2;
  gain = gain_;
  //make the buffer able to store 1 second of audio
  delay_buff = std::make_unique<deque>(delay, 0.0);
}

AllpassFilter::outType AllpassFilter::do_filtering(outType new_x) {
  auto &g = *delay_buff.get();
  auto g_out = g.back();
  g.pop_back();
  auto g_in = new_x + gain*g_out;
  g.push_front(g_in);
  auto y = -gain*g_in + g_out;

  //return the newest value for y
  return y;
}