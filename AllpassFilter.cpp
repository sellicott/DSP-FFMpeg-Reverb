#include "AllpassFilter.h"

AllpassFilter::AllpassFilter(size_t delay_, float gain_) {

  delay = delay_ * 2;
  gain = gain_;
  //make the buffer able to store 1 second of audio
  input_samples = std::make_unique<std::deque<double>>(delay, 0.0);
  output_samples = std::make_unique<std::deque<double>>(delay, 0.0);
}

double AllpassFilter::do_filtering(double new_x) {
  auto &x = *input_samples.get();
  auto &y = *output_samples.get();

  // replace the oldest value for x
  x.pop_back();
  x.push_front(new_x);

  // implement the filter. 
  auto new_val = x.back() -gain*x.front() + gain*y.back();

  // get rid of the oldest value for y
  y.pop_back();
  y.push_front(new_val);

  //return the newest value for y
  return y.front();
}