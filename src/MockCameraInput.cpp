#include <MockCameraInput.h>

MockCameraInput::MockCameraInput()
    : m_poses(7, N_FRAMES), m_gray(N_FRAMES), m_depth(N_FRAMES) {

  m_lastFrameIdx = 0;
  load_data(DATA_PATH + "/associated.txt", m_gray, m_depth, m_poses, N_FRAMES);

  std::cout << "Load data done: " << N_FRAMES << " frames" << std::endl;
}

odometry::KeyFrame MockCameraInput::getLatestKeyframe() {
  if (m_lastFrameIdx == m_gray.size()) {
    m_lastFrameIdx = 0;
  }

  cv::Mat rgbImage = cv::Mat::zeros(480, 640, CV_8UC3);

  // convert to rgb
  cv::cvtColor(m_gray[m_lastFrameIdx], rgbImage, cv::COLOR_GRAY2BGR);
  rgbImage.convertTo(rgbImage, CV_8UC3);

  auto p_leftRGB = std::make_shared<cv::Mat>(rgbImage);
  auto p_leftDepth = std::make_shared<cv::Mat>(m_depth[m_lastFrameIdx]);

  Eigen::VectorXf pose_raw = m_poses.col(m_lastFrameIdx);
  Eigen::Quaternionf pose_quat(pose_raw[0], pose_raw[1], pose_raw[2],
                               pose_raw[3]);
  Eigen::Vector3f pose_translation = pose_raw.block<3, 1>(4, 0);

  odometry::Affine4f pose;
  pose.block<3, 3>(0, 0) = pose_quat.normalized().toRotationMatrix();
  pose.block<3, 1>(0, 3) = pose_translation;

  m_lastFrameIdx += 1;

  cv::Mat depthMask(rgbImage.size(), CV_8UC1);
  cv::randu(depthMask, 0, 2);

  auto p_depthMask = std::make_shared<cv::Mat>(depthMask);

  return odometry::KeyFrame(p_leftRGB, p_leftRGB, p_leftDepth, p_depthMask,
                            pose);
}

void MockCameraInput::load_data(std::string filename,
                                std::vector<cv::Mat> &gray,
                                std::vector<cv::Mat> &depth,
                                Eigen::MatrixXf &poses, int n_frames) {
  std::string line;
  std::ifstream file(filename);

  int counter = 0;
  if (file.is_open()) {
    while (std::getline(file, line) && counter < n_frames) {
      std::vector<std::string> items;
      std::string item;
      std::stringstream ss(line);

      while (std::getline(ss, item, ' ')) {
        items.push_back(item);
      }

      // -> load gray
      std::string filename_rgb = std::string(DATA_PATH + "/") + items[9];
      cv::Mat gray_8u = cv::imread(filename_rgb, cv::IMREAD_GRAYSCALE);

      if (gray_8u.empty()) {
        std::cout << "read img failed for: " << counter << std::endl;
        std::exit(-1);
      }

      gray_8u.convertTo(gray[counter], PixelType);
      // <-

      // -> load depth
      std::string filename_depth = std::string(DATA_PATH + "/") + items[11];
      cv::Mat depth_img = cv::imread(filename_depth, cv::IMREAD_UNCHANGED);

      if (depth_img.empty()) {
        std::cout << "read depth img failed for: " << counter << std::endl;
        std::exit(-1);
      }

      depth_img.convertTo(depth[counter], PixelType, 1.0f / 5000.0f);

      // -> pose
      Eigen::Vector3f t(std::stof(items[1]), std::stof(items[2]),
                        std::stof(items[3])); // <- translation T
      // Eigen::Quaternionf q(std::stof(items[7]), std::stof(items[4]),
      // std::stof(items[5]), std::stof(items[6])); // <- rotation in Eigen:
      // w,x,y,z Eigen::Vector3f a =
      // Eigen::AngleAxisf(q).angle()*Eigen::AngleAxisf(q).axis(); // <--
      // convert to axis angle

      // clang-format off
      poses.col(counter) << std::stof(items[7]),
                            std::stof(items[4]),
                            std::stof(items[5]),
                            std::stof(items[6]),
                            t(0), t(1), t(2);
      // clang-format on

      counter++;
    }
    file.close();
  }
  assert(counter == n_frames);
};
