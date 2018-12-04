#include "FIRFilter.h"

FIRFilter::FIRFilter(std::vector<float> taps_) :
  taps(taps_)
{

  delay = taps_.size()*2;
  //make the buffer able to store 1 second of audio
  input_samples = std::make_unique<deque>(delay, 0.0);
}

FIRFilter::outType FIRFilter::do_filtering(outType new_x) {
  auto &x = *input_samples.get();

  // replace the oldest value for x
  x.pop_back();
  x.push_front(new_x);

  double new_val = 0;
  // implement the filter. 
  for (auto i = 0; i < delay/2; ++i) {
    //multiply by 2 to account for LR channels
    new_val += taps[i]*x[2*i];
  }

  //return the newest value for y
  return new_val;
}