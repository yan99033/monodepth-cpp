#include <monodepth/monodepth.h>

#include <opencv2/opencv.hpp>

/*
TODO:
To test if your Monodepth frozen graph is working properly
1. create a folder with a bunch of images
2. set the proper path to your pre-trained model and images directory
3. define the number of images (in the beginning of for loop)
*/

int main()
{
  monodepth::MonoDepth m(512, 256, "/path/to/monodepth/model/model_city2kitti.pb");
  cv::Mat depth;

  std::string img_path = "/path/to/image/folder/";

  int number_of_images = 1000;

  for (int i=0; i<number_of_images; i++)
  {
    char index_buf[7]; sprintf(index_buf, "%06d", i); std::string index_s(index_buf);
    cv::Mat image(cv::imread(img_path + index_s + ".png"));
    // std::cout << "read image in test monodepth" << std::endl;
    assert(!image.empty());

    m.inference(image, depth);

    // convert disparity to depth
    depth = 0.3128f / (depth + 0.00001f);

    double min_val, max_val;
    cv::Mat depthmap_visual;
    cv::threshold(depth, depthmap_visual, 50.0, 50.0, cv::THRESH_TRUNC); // apply threshold
    cv::minMaxLoc(depthmap_visual, &min_val, &max_val);
    depthmap_visual = 255 * (depthmap_visual - min_val) / (max_val - min_val);
    depthmap_visual.convertTo(depthmap_visual, CV_8U);
    cv::applyColorMap(depthmap_visual, depthmap_visual, 2); //COLORMAP_JET

    cv::Size img_size = image.size();
    int height = img_size.height * 2;
    int width = img_size.width;
    cv::Mat full(height, width, CV_8UC3);
    cv::Mat top(full, cv::Rect(0, 0, img_size.width, img_size.height));
    image.copyTo(top);
    cv::Mat bottom(full, cv::Rect(0, img_size.height, img_size.width, img_size.height));
    depthmap_visual.copyTo(bottom);


    cv::namedWindow("FULL", CV_WINDOW_AUTOSIZE);
    cv::imshow("FULL", full);
    cv::waitKey(1);
  }

  return 0;
}
