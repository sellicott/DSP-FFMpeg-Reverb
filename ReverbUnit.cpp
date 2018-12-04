#include "ReverbUnit.h"

#include <iostream>


ReverbUnit::ReverbUnit(size_t outbuff_size_) : 
  reverb_gain(0.6),
  //filter1(3920, reverb_gain), 
  //filter2(327, -reverb_gain), 
  //filter3(109, reverb_gain),
  //filter4(37, -reverb_gain),
  //filter5(18, reverb_gain),
  filter1(4410, 0.6), 
  filter2(1470, -0.6), 
  filter3(490, 0.6),
  filter4(163, -0.6),
  filter5(54, 0.6),

  firFilter1({ 0.003369,0.002810,0.001758,0.000340,-0.001255,-0.002793,-0.004014,
    -0.004659,-0.004516,-0.003464,-0.001514,0.001148,0.004157,0.006986,0.009003,
    0.009571,0.008173,0.004560,-0.001120,-0.008222,-0.015581,-0.021579,-0.024323,
    -0.021933,-0.012904,0.003500,0.026890,0.055537,0.086377,0.115331,0.137960,
    0.150407,0.150407,0.137960,0.115331,0.086377,0.055537,0.026890,0.003500,
    -0.012904,-0.021933,-0.024323,-0.021579,-0.015581,-0.008222,-0.001120,
    0.004560,0.008173,0.009571,0.009003,0.006986,0.004157,0.001148,-0.001514,
    -0.003464,-0.004516,-0.004659,-0.004014,-0.002793,-0.001255,0.000340,
    0.001758,0.002810,0.003369 }),

  firFilter2({ -0.029352,-0.026647,-0.020740,-0.010829,0.003836,0.023530,0.047656,
    0.074492,0.101210,0.124222,0.139892,0.145460,0.139892,0.124222,0.101210,
    0.074492,0.047656,0.023530,0.003836,-0.010829,-0.020740,-0.026647,-0.029352 }),

  firFilter3({ 0.016054,0.011635,0.003607,-0.006493,-0.016564,-0.024185,-0.026940,
    -0.022788,-0.010466,0.010148,0.037786,0.069719,0.101973,0.129864,0.148852,
    0.155594,0.148852,0.129864,0.101973,0.069719,0.037786,0.010148,-0.010466,
    -0.022788,-0.026940,-0.024185,-0.016564,-0.006493,0.003607,0.011635,0.016054 })
{
  outbuff_size = outbuff_size_;
  delay1 = std::make_unique<std::deque<double>>(2*2940, 0.0);
  delay2 = std::make_unique<std::deque<double>>(2*2940, 0.0);
  delay3 = std::make_unique<std::deque<double>>(2*2940, 0.0);
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

double ReverbUnit::do_filtering(double new_x) {
  auto &d1 = *delay1.get();
  auto &d2 = *delay2.get();
  auto &d3 = *delay3.get();


  auto x = 0.7*new_x;
  //auto x = 0.7*new_x + d3.back();

  auto flt1 = firFilter2.do_filtering(x);
  //run through the all pass filters
  auto temp1 = filter1.do_filtering(flt1);
  auto temp2 = filter2.do_filtering(temp1);
  auto temp3 = filter3.do_filtering(temp2);
  auto flt2 = firFilter3.do_filtering(temp3);
  auto temp4 = filter4.do_filtering(flt2);
  //auto temp5 = filter5.do_filtering(temp4);

  auto y = firFilter1.do_filtering(temp4);


  d3.pop_back();
  d3.push_front(d2.back());
  d2.pop_back();
  d2.push_front(d1.back());
  d1.pop_back();
  d1.push_front(y);


  auto temp6 = y + 0.5*d1.back() + 0.25*d2.back() + 0.125*d3.back();
  //auto temp7 = firFilter2.do_filtering(temp6);
  return temp6;
}