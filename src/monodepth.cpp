#include <monodepth/monodepth.h>
#include <monodepth/my_utils.h>

#include <tensorflow/cc/ops/array_ops.h>
#include <tensorflow/core/framework/graph.pb.h>
#include <tensorflow/core/framework/tensor.h>
#include <tensorflow/core/lib/core/status.h>
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/protobuf/config.pb.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/public/session_options.h>
#include <tensorflow/core/util/command_line_flags.h>

namespace monodepth {
  // remove the namespace reference
  using tensorflow::Flag;
  using tensorflow::Tensor;
  using tensorflow::Status;
  using tensorflow::string;
  using tensorflow::int32;

class MonoDepth::impl
{
public:
  /// Constructor
  impl(const int img_width,
       const int img_height,
       const std::string graph_str);

  ~impl();

  /// Infer depth from image (implementation)
  bool inference(const cv::Mat& image, cv::Mat& depth);

private:
  /// Precompute masks for post processing
  void PrecomputeMasks();

  /// Convert image to input tensor, estimate depthmap, and convert output tensor to depthmap
  Status PreprocessImage(const cv::Mat& image);
  Status PostprocessImage(const tensorflow::Tensor& depth);

  /// Input placeholder size
  static const int input_width_ = 512;
  static const int input_height_ = 256;

  int img_width_;                                                         /// Width of the input placeholder
  int img_height_;                                                        /// Height of the input placeholder
  cv::Size original_size_;                                                /// Keep the original shape of the image (resize the image if necessary)
  tensorflow::Scope root;                                                 /// Scope of the variable
  cv::Mat mask1_;                                                         /// Precomputed mask for depth1
  cv::Mat mask2_;                                                         /// Precomputed mask for depth2
  tensorflow::Tensor images_;                                             /// Stacked image1 and image2 tensor
  tensorflow::Tensor image1_;                                             /// Allocated memory for image1
  cv::Mat cv_image1_;                                                     /// Shared memory of the image1 tensor
  float* image1_p_;                                                       /// Pointer to image1 tensor
  tensorflow::Tensor image2_;                                             /// Allocated memory for image2
  cv::Mat cv_image2_;                                                     /// Shared memory of the image2 tensor
  float* image2_p_;                                                       /// Pointer to image2 tensor
  std::vector<tensorflow::Tensor> depth_;                                 /// output from monodepth
  cv::Mat cv_depth_;                                                      /// Postprocessed depth as cv::Mat
  std::unique_ptr<tensorflow::Session> session_;                          /// Tensorflow Session
  tensorflow::SessionOptions session_options_;                            /// Tensorflow Session options
  tensorflow::GraphDef graph_def_;                                        /// Monodepth computation graph
  std::string input_name_;                                                /// Input placeholder
  std::string output_name_;                                               /// Output placeholder
};

// Constructor of the interface
MonoDepth::MonoDepth(const int img_width,
                     const int img_height,
                     const std::string graph_str) :
  p_impl_{util::make_unique<impl>(img_width, img_height, graph_str)} {}

// Constructor of the implementation
MonoDepth::impl::impl(const int img_width,
                     const int img_height,
                     const std::string graph_str) :
  img_width_(img_width),
  img_height_(img_height),
  root(tensorflow::Scope::NewRootScope())
{
  // Store the original image size
  original_size_ = cv::Size(img_width, img_height);

  // Precompute mask
  mask1_ = cv::Mat(input_height_, input_width_, CV_32FC1);
  mask2_ = cv::Mat(input_height_, input_width_, CV_32FC1);
  PrecomputeMasks();

  // Image placeholders
  images_ = tensorflow::Tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({2, input_height_, input_width_, 3}));
  image1_p_ = images_.flat<float>().data();
  image2_p_ = image1_p_ + input_width_ * input_height_ * 3;
  cv_image1_ = cv::Mat(input_height_, input_width_, CV_32FC3, image1_p_);
  cv_image2_ = cv::Mat(input_height_, input_width_, CV_32FC3, image2_p_);

  // Load graph
  TF_CHECK_OK(ReadBinaryProto(tensorflow::Env::Default(), graph_str, &graph_def_));

  // Initialize session
  session_options_.config.mutable_gpu_options()->set_allow_growth(true);
  (&session_)->reset(tensorflow::NewSession(session_options_));
  TF_CHECK_OK(session_->Create(graph_def_));

  // Input and output
  input_name_ = "input_image";
  output_name_ = "model/output_depth";
}

// Destructor (interface)
MonoDepth::~MonoDepth() = default;

// Destructor(Implementation)
MonoDepth::impl::~impl()
{
  // Kill session
  session_->Close();
}

//Link interface to implementation
bool MonoDepth::inference(const cv::Mat& image, cv::Mat& depth)
{
  return p_impl_->inference(image, depth);
}

void MonoDepth::impl::PrecomputeMasks()
{
  std::vector<float> row;
  float val = 0.0f;
  float step = 1.0f / (float)(input_width_ - 1); // assume that it is not divided by zero

  // Create a row
  for (size_t i=0; i<input_width_; i++)
  {
    row.push_back(val);
    val += step;
  }

  // Create the masks with the row
  for (size_t i=0; i<input_height_; i++)
  {
    int offset = input_width_ * i;
    float* mask_ptr = (float*)mask1_.data + offset;
    memcpy(mask_ptr, row.data(), row.size()*sizeof(float));
  }

  mask1_ = 20.0f * (mask1_ - 0.05f);
  // clip the value [0-1]
  cv::threshold(mask1_, mask1_, 1.0f, 1.0f, cv::THRESH_TRUNC);
  cv::threshold(mask1_, mask1_, 0.0f, 1.0f, cv::THRESH_TOZERO);
  double min_val, max_val;
  cv::minMaxLoc(mask1_, &min_val, &max_val);
  // Compute the masks
  mask1_ = 1.0f - mask1_;
  cv::flip(mask1_, mask2_, 1);
}

bool MonoDepth::impl::inference(const cv::Mat& image, cv::Mat& depth)
{
  // Resize and flip the image
  TF_CHECK_OK(PreprocessImage(image));

  // Feed image to the network
  std::vector<std::pair<std::string, Tensor>> input = {std::make_pair(input_name_, images_)};
  TF_CHECK_OK(session_->Run(input, {output_name_}, {}, &depth_ ));

  // Postprocess depthmap
  TF_CHECK_OK(PostprocessImage(depth_[0]));

  cv_depth_.copyTo(depth);

  return true;
  /*
  direct copy of image to tensor: https://github.com/tensorflow/tensorflow/issues/8033
  */
}

Status MonoDepth::impl::PreprocessImage(const cv::Mat& image)
{
  // Store the image size
  // original_size_ = image.size();

  // Resize and flip image
  cv::Mat resized_img, resized_flipped_img;
  cv::resize(image, resized_img, cv::Size(input_width_, input_height_));
  resized_img.convertTo(cv_image1_, CV_32FC3, 1/255.0);
  cv::flip(cv_image1_, cv_image2_, 1);

  return Status::OK();
}

Status MonoDepth::impl::PostprocessImage(const tensorflow::Tensor& depth)
{
  // Combine the two depthmaps (original and flipped)
  cv::Mat cv_depth2;
  float* depth1_p = depth_[0].flat<float>().data();
  float* depth1_flipped_p = depth1_p + input_width_ * input_height_;
  cv::Mat cv_depth1 = cv::Mat(input_height_, input_width_, CV_32FC1, depth1_p);
  cv::Mat cv_depth1_flipped = cv::Mat(input_height_, input_width_, CV_32FC1, depth1_flipped_p);
  cv::flip(cv_depth1_flipped, cv_depth2, 1);
  cv_depth_ = 0.5f * (cv_depth1 + cv_depth2);
  cv_depth_ = cv_depth_.mul(1.0f - mask1_ - mask2_) + mask2_.mul(cv_depth1) + mask1_.mul(cv_depth2);

  // Upsize the image to original Shape
  cv::resize(cv_depth_, cv_depth_, original_size_);

  return Status::OK();
}

} // namespace monodepth
