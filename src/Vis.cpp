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

void Vis::setDataSource(DataGenerator *source) {
  m_dataSource = source;
  std::cout << "DataSource: " << source << std::endl;
  std::cout << "m_someValue: " << source->m_someValue << std::endl;
}

void Vis::initUI() {
  using namespace nanogui;

  nanogui::init();
  auto *screen = new VisScreen({1000, 750}, "NanoGUI test");
  screen->setLayout(
      new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 10));

  auto imageWindow = new Window(screen, "RGB Left");
  imageWindow->setLayout(
      new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

  GLuint rgbLeftTexId = getTextureForMat(m_frames["rgb_l"]);

  auto imageView = new ImageView(imageWindow, rgbLeftTexId);
  imageView->setFixedSize({300, 200});

  // Use redraw to reload images & points from data sources
  screen->onUpdate([this, rgbLeftTexId]() {
    double currentTime = glfwGetTime();

    if (currentTime - m_lastFrameTime >= 1.0) {
      m_fps = m_numElapsedFrames;
      m_numElapsedFrames = 0;
      m_lastFrameTime = glfwGetTime();

      std::cout << "FPS: " << m_fps << std::endl;
    }

    m_numElapsedFrames += 1;
    cv::Mat newImg = *m_dataSource->m_activeImage;

    glBindTexture(GL_TEXTURE_2D, rgbLeftTexId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, newImg.cols, newImg.rows, GL_BGR,
                    GL_UNSIGNED_BYTE, newImg.ptr());
  });

  // To test layouting...
  auto imageWindow2 = new Window(screen, "RGB Right");
  imageWindow2->setLayout(
      new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

  // Display vtk
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
    trajectoryView->addPoint(nanogui::Vector3f(
        (rand() % 100) / 100.0f * 2 - 1, (rand() % 100) / 100.0f * 2 - 1,
        (rand() % 100) / 100.0f * 2 - 1));
  });

  screen->performLayout();
  screen->drawAll();
  screen->setVisible(true);
}

void Vis::startUI() {
  nanogui::mainloop(30);
  nanogui::shutdown();
}