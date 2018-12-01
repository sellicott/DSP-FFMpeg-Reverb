#include "ReverbUnit.h"

#include <iostream>


ReverbUnit::ReverbUnit(size_t outbuff_size_) : 
  reverb_gain(0.5),
  filter1(3920, reverb_gain), 
  filter2(327, reverb_gain), 
  filter3(109, reverb_gain),
  filter4(531, reverb_gain),
  filter5(227, reverb_gain),
  comb1(1687, 0.773),
  comb2(1601, 0.802), 
  //comb2(44100, 0.9),
  comb3(2054, 0.753),
  comb4(2251, 0.733)
  {
  outbuff_size = outbuff_size_;
  delay1 = std::make_unique<std::deque<double>>(2*2940);
  delay2 = std::make_unique<std::deque<double>>(2*2940);
  delay3 = std::make_unique<std::deque<double>>(2*2940);
  std::fill(delay1->begin(), delay1->end(), 0.0);
  std::fill(delay2->begin(), delay2->end(), 0.0);
  std::fill(delay3->begin(), delay3->end(), 0.0);
}

uint8_t* ReverbUnit::get_samples(uint8_t* samples, size_t num_samples) {

  //convert the uint8_t samples to floating point
  auto float_samples = reinterpret_cast<float*>(samples);
  auto num_float_samples = num_samples/sizeof(float);

  for (auto i = 0; i < num_float_samples; i++) {
    //filters on!
    auto left_sample = float_samples[i];
    float_samples[i] = do_filtering(left_sample) + 1.5*left_sample;
    //std::cout << left_sample << ' ' << float_samples[i] << '\n' ;
    ++i;
    auto right_sample = float_samples[i];
    float_samples[i] = do_filtering(right_sample) + 1.5*right_sample;
  }

  return reinterpret_cast<uint8_t*>(samples);  
}

double ReverbUnit::do_filtering(double new_x) {
  auto &d1 = *delay1.get();
  auto &d2 = *delay2.get();
  auto &d3 = *delay3.get();

  //auto x = new_x + 0.5*d3[0];
  auto x = new_x;

  //run through the all pass filters
  
  auto temp1 = filter1.do_filtering(x);
  auto temp2 = filter2.do_filtering(temp1);
  auto temp3 = filter3.do_filtering(temp2);
  auto temp4 = filter4.do_filtering(temp3);
  auto temp5 = filter5.do_filtering(temp4);

  //run through the comb filters
  auto out1 = comb1.do_filtering(temp5);
  auto out2 = comb2.do_filtering(temp5);
  auto out3 = comb3.do_filtering(temp5);
  auto out4 = comb4.do_filtering(temp5);

  d3.pop_back();
  d3.push_front(d2.back());
  d2.pop_back();
  d2.push_front(d1.back());
  d1.pop_back();
  d1.push_front(temp5);
  

  //auto y = temp5;
  auto y = temp5 + 0.5*d1.back() + 0.25*d2.back() + 0.125*d3.back();
  //auto y = out1 + out2 + out3 + out4;

  //auto y = comb2.do_filtering(x);
  

  return y;
}