#include <Vis.h>
#include <string> // TODO: Remove

GLuint getTextureId() {
  GLuint imageTexId;
  glGenTextures(1, &imageTexId);
  glBindTexture(GL_TEXTURE_2D, imageTexId);

  cv::Mat blankImgData = cv::Mat::zeros(480, 640, CV_8UC3);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, blankImgData.cols, blankImgData.rows,
               0, GL_BGR, GL_UNSIGNED_BYTE, blankImgData.ptr());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  return imageTexId;
}

void bindMatToTexture(const cv::Mat &image, GLuint textureId) {
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.cols, image.rows, GL_BGR,
                  GL_UNSIGNED_BYTE, image.ptr());
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
  auto screen = new VisScreen({1000, 750}, "NanoGUI test");
  screen->setLayout(
      new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 10));

  auto rgbImageWindow = new Window(screen, "RGB");
  rgbImageWindow->setLayout(
      new BoxLayout(Orientation::Horizontal, Alignment::Middle, 5, 5));

  // Reserve some Textures for later images
  m_rgbLeftTexId = getTextureId();
  m_rgbRightTexId = getTextureId();
  m_depthLeftTexId = getTextureId();

  auto rgbLeftView = new ImageView(rgbImageWindow, m_rgbLeftTexId);
  rgbLeftView->setFixedSize({300, 200});

  auto rgbRightView = new ImageView(rgbImageWindow, m_rgbRightTexId);
  rgbRightView->setFixedSize({300, 200});

  // To test layouting...
  auto imageWindow2 = new Window(screen, "RGB Right");
  imageWindow2->setLayout(
      new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

  // Display the 3d trajectory
  auto trajectoryView = new TrajectoryView(imageWindow2);
  m_view = trajectoryView;

  trajectoryView->setSize({400, 400});

  Button *b1 = new Button(imageWindow2, "Random Rotation");
  b1->setCallback([trajectoryView, this]() {
    trajectoryView->setRotation(Vector3f((rand() % 100) / 100.0f,
                                         (rand() % 100) / 100.0f,
                                         (rand() % 100) / 100.0f));
  });

  Button *b_zoom = new Button(imageWindow2, "Increase Zoom");
  b_zoom->setCallback([trajectoryView]() {
    auto zoom = trajectoryView->getZoom();

    trajectoryView->setZoom(zoom * 1.1);
  });

  Button *b_zoom2 = new Button(imageWindow2, "Decrease Zoom");
  b_zoom2->setCallback([trajectoryView]() {
    auto zoom = trajectoryView->getZoom();

    trajectoryView->setZoom(zoom * 0.9);
  });

  Button *b_addPoint = new Button(imageWindow2, "Add outlier point");
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

    // Draw buffered keyframes
    for (odometry::KeyFrame &keyframe : m_keyframeBuffer) {
      cv::Mat leftRGB = keyframe.GetLeftImg();
      cv::Mat rightRGB = keyframe.GetRightImg();

      bindMatToTexture(leftRGB, m_rgbLeftTexId);
      bindMatToTexture(rightRGB, m_rgbRightTexId);

      cv::Mat leftDepth = keyframe.GetLeftDep(); // 32FC1, min/max:0/9.87
      cv::Mat leftValue = keyframe.GetLeftVal();

      odometry::Affine4f absolutePose = keyframe.GetAbsoPose();

      double min, max;
      cv::minMaxLoc(leftDepth, &min, &max);

      std::cout << "Depth Min/Max:" << min << "/" << max << std::endl;
      std::cout << "Depth Format:" << type2str(leftDepth.type()) << std::endl;

      int nChannels = leftDepth.channels();
      int nRows = leftDepth.rows;
      int nCols = leftDepth.cols * nChannels;

      int yi, xi;
      float *p_pixel;

      std::vector<Vector3f> projectedPoints;

      for (yi = 0; yi < nRows; yi++) {
        p_pixel = leftDepth.ptr<float>(yi);

        for (xi = 0; xi < nCols; xi++) {
          float zi = p_pixel[xi];
          // std::cout << "Depth at (" << yi << "," << xi << ") = " << zi
          //           << std::endl;

          if (zi == 0) {
            continue;
          }

          Vector4f pointImage(xi, yi, zi, 1);
          Vector4f pointCamera = m_intrinsicsInv * pointImage;
          Vector4f pointWorld = absolutePose * pointCamera;

          projectedPoints.push_back(
              {pointWorld.x(), pointWorld.z(), pointWorld.y()});
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
