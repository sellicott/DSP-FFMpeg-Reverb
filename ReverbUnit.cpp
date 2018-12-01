#include "ReverbUnit.h"

#include <iostream>


ReverbUnit::ReverbUnit(size_t outbuff_size_) : 
  reverb_gain(0.6),
  filter1(3920, reverb_gain), 
  filter2(327, reverb_gain), 
  filter3(109, -reverb_gain),
  filter4(36, reverb_gain),
  filter5(517, -0.55),

  firFilter1({0.0028603, -0.0025789, -0.0112201, -0.0191014, -0.0214864, -0.0140984,
   0.0055837, 0.0372531, 0.0772406, 0.1188109, 0.1534783, 0.1732583, 0.1732583,
   0.1534783, 0.1188109, 0.0772406, 0.0372531, 0.0055837, -0.0140984, -0.0214864
  -0.0191014, -0.0112201, -0.0025789, 0.0028603}),

  firFilter2({0.0028603, -0.0025789, -0.0112201, -0.0191014, -0.0214864, -0.0140984,
   0.0055837, 0.0372531, 0.0772406, 0.1188109, 0.1534783, 0.1732583, 0.1732583,
   0.1534783, 0.1188109, 0.0772406, 0.0372531, 0.0055837, -0.0140984, -0.0214864
  -0.0191014, -0.0112201, -0.0025789, 0.0028603}),

  comb1(1687, 0.773),
  comb2(1601, 0.802), 
  //comb2(44100, 0.9),
  comb3(2054, 0.753),
  comb4(2251, 0.733)
  {
  outbuff_size = outbuff_size_;
  delay1 = std::make_unique<std::deque<double>>(2*2940, 0.0);
  delay2 = std::make_unique<std::deque<double>>(2*2940, 0.0);
  delay3 = std::make_unique<std::deque<double>>(2*2940, 0.0);
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

  auto x = new_x;

  //run through the all pass filters
  auto temp1 = filter1.do_filtering(x);
  auto temp2 = filter2.do_filtering(temp1);
  auto temp3 = filter3.do_filtering(temp2);
  auto temp4 = filter4.do_filtering(temp3);
  auto temp5 = filter5.do_filtering(temp4);

  d3.pop_back();
  d3.push_front(d2.back());
  d2.pop_back();
  d2.push_front(d1.back());
  d1.pop_back();
  d1.push_front(temp5);

  auto temp6 = temp5 + 0.5*d1.back() + 0.25*d2.back() + 0.125*d3.back();
  auto temp7 = firFilter2.do_filtering(temp6);

  auto y = firFilter1.do_filtering(temp7);
  return y;
}