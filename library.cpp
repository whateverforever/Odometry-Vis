#include "library.h"

GLuint getTextureForMat(cv::Mat &mat) {
    GLuint imageTexId;
    glGenTextures(1, &imageTexId);
    glBindTexture(GL_TEXTURE_2D, imageTexId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, mat.ptr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return imageTexId;
}

Vis::Vis() {
    m_frames["rgb_l"] = cv::Mat::zeros(600, 400, CV_8UC3);
}

void Vis::calledFromOutSide() {
    std::cout << "LOLWASALTER" << std::endl;
}

void Vis::initUI() {
    using namespace nanogui;

    nanogui::init();
    Screen *screen = new Screen({1000, 750}, "NanoGUI test");
    screen->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 10));

    auto imageWindow = new Window(screen, "RGB Left");
    imageWindow->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

    GLuint rgbLeftTexId = getTextureForMat(m_frames["rgb_l"]);

    auto imageView = new ImageView(imageWindow, rgbLeftTexId);
    imageView->setFixedSize({300,200});

    // auto loadNewImgBtn = new Button(imageWindow, "Load new frame");
    // loadNewImgBtn->setCallback([rgbLeftTexId, image_2]() {
    //     glBindTexture(GL_TEXTURE_2D, rgbLeftTexId);
    //     glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, image_2.cols, image_2.rows, GL_BGR, GL_UNSIGNED_BYTE, image_2.ptr());
    // });

    // To test layouting...
    auto imageWindow2 = new Window(screen, "RGB Right");
    imageWindow2->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

    // Display vtk
    auto vtkCanvas = new TrajectoryView(imageWindow2);
    vtkCanvas->setSize({400,400});

    Button *b1 = new Button(imageWindow2, "Random Rotation");
    b1->setCallback([vtkCanvas]() {
      vtkCanvas->setRotation(nanogui::Vector3f((rand() % 100) / 100.0f,
                                               (rand() % 100) / 100.0f,
                                               (rand() % 100) / 100.0f));
    });

    Button *b2 = new Button(imageWindow2, "Add new point");
    b2->setCallback([vtkCanvas]() {
      vtkCanvas->addPoint(nanogui::Vector3f((rand() % 100) / 100.0f * 2 - 1,
                                            (rand() % 100) / 100.0f * 2 - 1,
                                            (rand() % 100) / 100.0f * 2 - 1));
    });

    screen->performLayout();
    screen->drawAll();
    screen->setVisible(true);
}

void Vis::startUI() {
    nanogui::mainloop();
    nanogui::shutdown();
}