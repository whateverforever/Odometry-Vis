#include <DataGen.h>

float RandomFloat(float a, float b) {
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = b - a;
  float r = random * diff;
  return a + r;
}

DataGenerator::DataGenerator() {
  m_image_1 = cv::imread(
      "../data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png",
      CV_LOAD_IMAGE_COLOR); // Read the file
  m_image_2 = cv::imread(
      "../data/rgbd_dataset_freiburg3_teddy/rgb/1341841879.367159.png",
      CV_LOAD_IMAGE_COLOR); // Read the file
}

odometry::KeyFrame DataGenerator::getLatestKeyframe() {
  auto p_leftRGB = std::make_shared<cv::Mat>(m_image_1);
  auto p_rightRGB = std::make_shared<cv::Mat>(m_image_2);
  auto p_leftDepth = std::make_shared<cv::Mat>(m_image_1);
  auto p_leftValue = std::make_shared<cv::Mat>(m_image_2);

  // clang-format off
  odometry::Affine4f pose;
  pose <<   1, 0, 0, RandomFloat(-4,4),
            0, 1, 0, RandomFloat(-4,4),
            0, 0, 1, RandomFloat(-4,4),
            0, 0, 0, 1;
  // clang-format on

  nanogui::Matrix3f m;
  m = Eigen::AngleAxisf(RandomFloat(0, 1) * M_PI, nanogui::Vector3f::UnitX()) *
      Eigen::AngleAxisf(RandomFloat(0, 1) * M_PI, nanogui::Vector3f::UnitY()) *
      Eigen::AngleAxisf(RandomFloat(0, 1) * M_PI, nanogui::Vector3f::UnitZ());

  pose.block<3, 3>(0, 0) = m;

  auto kframe = odometry::KeyFrame(p_leftRGB, p_rightRGB, p_leftDepth,
                                   p_leftValue, pose);
  return kframe;
}
