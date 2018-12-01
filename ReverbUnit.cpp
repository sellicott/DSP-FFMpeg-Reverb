#include "ReverbUnit.h"

#include <iostream>


ReverbUnit::ReverbUnit(size_t outbuff_size_) {
  outbuff_size = outbuff_size_;
  //make the buffer able to store 1 second of audio
  input_samples = std::make_unique<CircularBuffer<float>>(sample_rate);
  output_samples = std::make_unique<CircularBuffer<float>>(sample_rate);
  
  // fill the buffers with zeros
  std::fill(input_samples->begin(), input_samples->end(), 0.0);
  std::fill(output_samples->begin(), output_samples->end(), 0.0);
}

uint8_t* ReverbUnit::get_samples(uint8_t* samples, size_t num_samples) {

  //convert the uint8_t samples to floating point
  auto float_samples = reinterpret_cast<float*>(samples);
  auto num_float_samples = num_samples/sizeof(float);

  for (auto i = 0; i < num_float_samples; i++) {
    //filters on!
    auto left_sample = float_samples[i];
    float_samples[i] = do_filtering(left_sample) + 0.5*left_sample;
    //std::cout << left_sample << ' ' << float_samples[i] << '\n' ;
    ++i;
    auto right_sample = float_samples[i];
    float_samples[i] = do_filtering(right_sample) + 0.5*right_sample;
  }

  return reinterpret_cast<uint8_t*>(samples);  
}

float ReverbUnit::do_filtering(float new_x) {
  auto a1 = 0.5;
  auto &x = *input_samples.get();
  auto &y = *output_samples.get();

  // replace the oldest value for x
  x.push_back(new_x);

  // implement the filter. 
  // note that the oldest values of x and y are at index 0
  auto last_x = x.size()-1;
  auto new_val = x[0] - a1*x.back() + a1*y[0];
  //auto new_val = new_x;

  // get rid of the oldest value for y
  y.push_back(new_val);

  //return the newest value for y
  return y.back();
}