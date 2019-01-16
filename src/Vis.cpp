#include "Vis.h"

GLuint getTextureForMat(cv::Mat &mat) {
  GLuint imageTexId;
  glGenTextures(1, &imageTexId);
  glBindTexture(GL_TEXTURE_2D, imageTexId);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_BGR,
               GL_UNSIGNED_BYTE, mat.ptr());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  return imageTexId;
}

Vis::Vis() {
  m_frames["rgb_l"] = cv::Mat::zeros(480, 640, CV_8UC3);
  std::cout << "This: " << this << std::endl;

  m_lastFrameTime = glfwGetTime();
}

void Vis::addTrajectoryPoint(nanogui::Vector3f point) {
  std::cout << "lel" << std::endl;
  m_pointBuffer.push_back(point);

  // m_view->addPoint(point);
}

void Vis::loadNewestKeyframe(const odometry::KeyFrame &keyframe) {
  m_keyframeBuffer.push_back(keyframe);
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

  GLuint rgbLeftTexId = getTextureForMat(m_frames["rgb_l"]);

  auto rgbLeftView = new ImageView(rgbImageWindow, rgbLeftTexId);
  rgbLeftView->setFixedSize({300, 200});

  auto rgbRightView = new ImageView(rgbImageWindow, rgbLeftTexId);
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
    trajectoryView->setRotation(nanogui::Vector3f((rand() % 100) / 100.0f,
                                                  (rand() % 100) / 100.0f,
                                                  (rand() % 100) / 100.0f));
  });

  Button *b2 = new Button(imageWindow2, "Add new point");
  b2->setCallback([trajectoryView]() {

    auto newPoint = nanogui::Vector3f(
        (rand() % 100) / 100.0f * 2 - 1, (rand() % 100) / 100.0f * 2 - 1,
        (rand() % 100) / 100.0f * 2 - 1);

    trajectoryView->addPoint(newPoint);
  });

  // Use redraw to reload images & points from data sources
  screen->onUpdate([this, rgbLeftTexId, trajectoryView]() {
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

    // Load new images
    // cv::Mat newImg = *m_dataSource->m_activeImage;

    // glBindTexture(GL_TEXTURE_2D, rgbLeftTexId);
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, newImg.cols, newImg.rows, GL_BGR,
    //                 GL_UNSIGNED_BYTE, newImg.ptr());

    // Load buffered keyframes
    for(const odometry::KeyFrame &keyframe : m_keyframeBuffer) {
      std::cout << "Processing new keyframe" << std::endl;
    }

    m_keyframeBuffer.clear();

  });

  screen->performLayout();
  screen->drawAll();
  screen->setVisible(true);

  nanogui::mainloop(30);
  nanogui::shutdown();
}