#ifndef MONODEPTH_H_
#define MONODEPTH_H_

#include <opencv2/opencv.hpp>

namespace monodepth {

class MonoDepth
{
public:
  // constructor 
  MonoDepth(const int img_width,
            const int img_height,
            const std::string graph_str);

  // destructor
  ~MonoDepth();

  bool inference(const cv::Mat& image, cv::Mat& depth);

private:
  /// forward declare the implementation
  class impl;
  std::unique_ptr<impl> p_impl_;
};

} // namespace monodepth

#endif
