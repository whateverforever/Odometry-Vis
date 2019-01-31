#include <Vis.h>
#include <string> // TODO: Remove

GLuint getTextureId() {
  GLuint imageTexId;
  glGenTextures(1, &imageTexId);
  glBindTexture(GL_TEXTURE_2D, imageTexId);

  // TODO, FIXME: Change to generic image size
  cv::Mat blankImgData = cv::Mat::zeros(376, 1241, CV_8UC3);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, blankImgData.cols, blankImgData.rows,
               0, GL_BGR, GL_UNSIGNED_BYTE, blankImgData.ptr());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  return imageTexId;
}

void singleChannelToColorMap(const cv::Mat &inImg, cv::Mat &outImage,
                             double minVal, double maxVal) {
  cv::Mat tmpImage;

  inImg.convertTo(tmpImage, CV_8UC1, 255 / (maxVal - minVal),
                  -255 * minVal / (maxVal - minVal));

  applyColorMap(tmpImage, outImage, cv::COLORMAP_PARULA);
}

void bindMatToTexture(const cv::Mat &image, GLuint textureId,
                      bool colorMap = false) {
  glBindTexture(GL_TEXTURE_2D, textureId);

  uint imageColor;
  uint dataFormat;

  cv::Mat tmpImage;
  cv::Mat colorMappedImage;

  if (colorMap) {
    // Manually set min/max to fit the important data (<4m for teddy) into the
    // 8UC1
    // double minVal = 0.2;
    // double maxVal = 4;

    double minVal, maxVal;
    cv::minMaxLoc(image, &minVal, &maxVal);

    // singleChannelToColorMap(image, colorMappedImage, minVal, maxVal);

    imageColor = GL_RED;
    dataFormat = GL_FLOAT;

    colorMappedImage = image;
    // GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ZERO};
    // glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

  } else {
    imageColor = GL_BGR;
    dataFormat = GL_UNSIGNED_BYTE;

    colorMappedImage = image;
  }

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, colorMappedImage.cols,
                  colorMappedImage.rows, imageColor, dataFormat,
                  colorMappedImage.ptr());
}

Vis::Vis(float fx, float fy, float f_theta, float cx, float cy) {
  m_lastFrameTime = glfwGetTime();

  // clang-format off
  m_intrinsics = nanogui::Matrix4f::Identity();
  m_intrinsics <<   fx, f_theta, cx, 0,
                    0,       fy, cy, 0,
                    0,        0,  1, 0,
                    0,        0,  0, 1;

  m_intrinsicsInv << 1/fx, -f_theta/(fx*fy), -(cx*fy -cy*f_theta)/(fx*fy), 0,
                        0,             1/fy,                       -cy/fy, 0,
                        0,                0,                            1, 0,
                        0,                0,                            0, 1;
  // clang-format on
  std::cout << "Set intrinsics:\n" << m_intrinsics << std::endl;
}

void Vis::loadNewestKeyframe(const odometry::KeyFrame &keyframe) {
  m_keyframeBuffer.push_back(keyframe);
}

// TODO: Remove
std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch (depth) {
  case CV_8U:
    r = "8U";
    break;
  case CV_8S:
    r = "8S";
    break;
  case CV_16U:
    r = "16U";
    break;
  case CV_16S:
    r = "16S";
    break;
  case CV_32S:
    r = "32S";
    break;
  case CV_32F:
    r = "32F";
    break;
  case CV_64F:
    r = "64F";
    break;
  default:
    r = "User";
    break;
  }

  r += "C";
  r += (chans + '0');

  return r;
}

void Vis::start() {
  using namespace nanogui;

  nanogui::init();
  VisScreen *screen = new VisScreen({1280, 720}, "NanoGUI test");
  screen->setLayout(
      new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 10));

  // Reserve some Textures for later images
  m_rgbLeftTexId = getTextureId();
  m_rgbRightTexId = getTextureId();

  /* Left Window with Camera Images */

  auto rgbImageWindow = new Window(screen, "RGB");
  rgbImageWindow->setLayout(
      new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

  Widget *firstRow = new Widget(rgbImageWindow);
  firstRow->setLayout(
      new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 6));

  auto rgbLeftView = new ImageView(firstRow, m_rgbLeftTexId);
  rgbLeftView->setFixedSize({400, 300});
  // Weird nanogui behaviour, see
  // https://nanogui.readthedocs.io/en/latest/api/class_nanogui__Widget.html?highlight=widget#_CPPv2N7nanogui6Widget12setFixedSizeERK8Vector2i
  rgbLeftView->setSize(rgbLeftView->fixedSize());
  rgbLeftView->fit();
  auto rgbRightView = new ImageView(firstRow, m_rgbRightTexId);
  rgbRightView->setFixedSize({400, 300});
  rgbRightView->setSize(rgbRightView->fixedSize());
  rgbRightView->fit();

  // To test layouting...
  auto viewportWindow = new Window(screen, "Viewport");
  viewportWindow->setLayout(
      new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

  // Display the 3d trajectory
  auto trajectoryView = new TrajectoryView(viewportWindow);
  m_view = trajectoryView;

  trajectoryView->setSize({400, 400});

  Button *b1 = new Button(viewportWindow, "Random Rotation");
  b1->setCallback([trajectoryView, this]() {
    trajectoryView->setRotation(Vector3f((rand() % 100) / 100.0f,
                                         (rand() % 100) / 100.0f,
                                         (rand() % 100) / 100.0f));
  });

  Button *b_zoom = new Button(viewportWindow, "Increase Zoom");
  b_zoom->setCallback([trajectoryView]() {
    auto zoom = trajectoryView->getZoom();

    trajectoryView->setZoom(zoom * 1.1);
  });

  Button *b_zoom2 = new Button(viewportWindow, "Decrease Zoom");
  b_zoom2->setCallback([trajectoryView]() {
    auto zoom = trajectoryView->getZoom();

    trajectoryView->setZoom(zoom * 0.9);
  });

  Button *b_addPoint = new Button(viewportWindow, "Add outlier point");
  b_addPoint->setCallback([trajectoryView]() {
    auto newPoint = Vector3f(10, 0, 10);
    trajectoryView->addPoint(newPoint);
  });

  // Use redraw to reload images & points from data sources
  screen->onUpdate([this, trajectoryView]() {
    /******** <FPS> ********/
    double currentTime = glfwGetTime();
    if (currentTime - m_lastFrameTime >= 1.0) {
      m_fps = m_numElapsedFrames;
      m_numElapsedFrames = 0;
      m_lastFrameTime = glfwGetTime();

      std::cout << "FPS: " << m_fps << std::endl;
    }
    m_numElapsedFrames += 1;
    /******** </FPS> ********/

    std::cout << "Going to draw " << m_keyframeBuffer.size() << " keyframes.."
              << std::endl;
    // Draw buffered keyframes
    for (odometry::KeyFrame &keyframe : m_keyframeBuffer) {
      cv::Mat leftRGB = keyframe.GetLeftImg();
      cv::Mat rightRGB = keyframe.GetRightImg();
      cv::Mat leftDepth = keyframe.GetLeftDep(); // 32FC1, min/max:0/9.87
      cv::Mat leftValue = keyframe.GetLeftVal(); // 8UC1, mask for depth

      std::cout << "Depth is " << type2str(leftDepth.type()) << std::endl;

      cv::Mat depthColored;
      singleChannelToColorMap(leftDepth, depthColored, 0.2, 4.0);

      depthColored.copyTo(leftRGB, leftValue);

      bindMatToTexture(leftRGB, m_rgbLeftTexId);
      bindMatToTexture(rightRGB, m_rgbRightTexId);

      odometry::Affine4f absolutePose = keyframe.GetAbsoPose();

      // Swapping Y and Z to convert coordinates to a right-hand system
      Matrix4f swapYZ;
      swapYZ.block<3, 3>(0, 0) =
          Eigen::AngleAxisf(-3.14 / 2, Vector3f::UnitX()).toRotationMatrix();
      // absolutePose = swapYZ * absolutePose; // TODO: remove, commented out
      // for kitti

      Matrix3f rot = absolutePose.block<3, 3>(0, 0);
      Vector3f tra = absolutePose.block<3, 1>(0, 3);

      Matrix4f poseInv;
      poseInv.block<3, 3>(0, 0) = rot.transpose();
      poseInv.block<3, 1>(0, 3) = -rot.transpose() * tra;
      poseInv(3, 3) = 1;

      int nChannels = leftDepth.channels();
      int nRows = leftDepth.rows;
      int nCols = leftDepth.cols * nChannels;

      int yi, xi;
      float *p_pixel;

      std::vector<Vector3f> projectedPoints;

      // Parameter to control how dense the reprojection should be
      const int useEveryN = 0;
      int skipCount = 0;

      for (yi = 0; yi < nRows; yi++) {
        p_pixel = leftDepth.ptr<float>(yi);

        for (xi = 0; xi < nCols; xi++) {
          float zi = p_pixel[xi];
          // std::cout << "Depth at (" << yi << "," << xi << ") = " << zi
          //           << std::endl;

          if (zi == 0) {
            continue;
          }

          if (skipCount < useEveryN) {
            skipCount++;
            continue;
          }

          skipCount = 0;

          Vector4f pointImage(xi, yi, zi, 1);
          Vector4f pointCamera = m_intrinsicsInv * pointImage;
          Vector4f pointWorld = absolutePose * pointCamera;

          projectedPoints.push_back(
              {pointWorld.x(), pointWorld.y(), pointWorld.z()});
        }
      }

      m_view->addPose(absolutePose);
      m_view->addPoints(projectedPoints);
    }

    m_keyframeBuffer.clear();
  });

  screen->performLayout();
  screen->drawAll();
  screen->setVisible(true);

  nanogui::mainloop(30);
  nanogui::shutdown();
}
