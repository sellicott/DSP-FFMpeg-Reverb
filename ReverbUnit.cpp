#include "ReverbUnit.h"

#include <iostream>


ReverbUnit::ReverbUnit(size_t outbuff_size_) : 
  reverb_gain(0.6),
  filter1(4410, 0.6), 
  filter2(1470, -0.6), 
  filter3(490, 0.6),
  filter4(163, -0.6),

  firFilter({ 0.003369,0.002810,0.001758,0.000340,-0.001255,-0.002793,-0.004014,
    -0.004659,-0.004516,-0.003464,-0.001514,0.001148,0.004157,0.006986,0.009003,
    0.009571,0.008173,0.004560,-0.001120,-0.008222,-0.015581,-0.021579,-0.024323,
    -0.021933,-0.012904,0.003500,0.026890,0.055537,0.086377,0.115331,0.137960,
    0.150407,0.150407,0.137960,0.115331,0.086377,0.055537,0.026890,0.003500,
    -0.012904,-0.021933,-0.024323,-0.021579,-0.015581,-0.008222,-0.001120,
    0.004560,0.008173,0.009571,0.009003,0.006986,0.004157,0.001148,-0.001514,
    -0.003464,-0.004516,-0.004659,-0.004014,-0.002793,-0.001255,0.000340,
    0.001758,0.002810,0.003369 })
{
  outbuff_size = outbuff_size_;
  delay = std::make_unique<deque>(3*2*2940, 0.0);
}

uint8_t* ReverbUnit::get_samples(uint8_t* samples, size_t num_samples) {

  //convert the uint8_t samples to floating point
  auto samples_cast = reinterpret_cast<int16_t*>(samples);
  auto num_samples_cast = num_samples/sizeof(int16_t);

  for (auto i = 0; i < num_samples_cast; i++) {
    //filters on!
    auto left_sample = samples_cast[i];
    auto filtered = reverb_gain*do_filtering(left_sample);
    samples_cast[i] = filtered + left_sample;
    //std::cout << left_sample << ' ' << filtered << '\n' ;
    ++i;
    auto right_sample = samples_cast[i];
    filtered = reverb_gain*do_filtering(right_sample);
    samples_cast[i] = filtered + right_sample;
  }

  return reinterpret_cast<uint8_t*>(samples);  
}

ReverbUnit::outType ReverbUnit::do_filtering(outType new_x) {
  auto &d = *delay.get();

  // the coefficient on the d.back() sets how long the reverb 
  // will sustain: larger = longer 
  auto x = 0.7*new_x + 0.25*d.back();

  //run through the all pass filters
  auto temp = filter1.do_filtering(x);
  temp = filter2.do_filtering(temp);
  temp = filter3.do_filtering(temp);
  temp = filter4.do_filtering(temp);
  temp = firFilter.do_filtering(temp);

  d.pop_back();
  d.push_front(temp);

  //add a bit of an FIR filter here, smooth the output
  auto y = temp + 0.5*d[2*2940] + 0.25*d[2*2*2940] + 0.125*d.back();
  return y;
}