#include "FIRFilter.h"

FIRFilter::FIRFilter(std::vector<float> taps_) :
  taps(taps_)
{

  delay = taps_.size()*2;
  //make the buffer able to store 1 second of audio
  input_samples = std::make_unique<std::deque<double>>(delay, 0.0);
  output_samples = std::make_unique<std::deque<double>>(delay, 0.0);
}

double FIRFilter::do_filtering(double new_x) {
  auto &x = *input_samples.get();
  auto &y = *output_samples.get();

  // replace the oldest value for x
  x.pop_back();
  x.push_front(new_x);

  double new_val = 0;
  // implement the filter. 
  for (auto i = 0; i < delay/2; ++i) {
    //multiply by 2 to account for LR channels
    new_val += taps[i]*x[2*i];
  }

  // get rid of the oldest value for y
  y.pop_back();
  y.push_front(new_val);

  //return the newest value for y
  return new_val;
}