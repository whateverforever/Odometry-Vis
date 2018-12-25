#include <iostream>
#include <stdlib.h>

#include <nanogui/nanogui.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vtkSmartPointer.h>

#include "trajectoryView.h"

using namespace nanogui;

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

int main(int /* argc */, char ** /* argv */) {

    // First, load an image with openCV and do stuff with it
    cv::Mat image, image_2;
    image = cv::imread("../data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    image_2 = cv::imread("../data/rgbd_dataset_freiburg3_teddy/rgb/1341841879.367159.png", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    // Second, display the image using nano
    nanogui::init();
    Screen *screen = new Screen({1000, 750}, "NanoGUI test");
    screen->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 10));

    auto imageWindow = new Window(screen, "RGB Left");
    imageWindow->setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle, 5, 5));

    GLuint imageTexId = getTextureForMat(image);

    auto imageView = new ImageView(imageWindow, imageTexId);
    imageView->setFixedSize({300,200});

    auto loadNewImgBtn = new Button(imageWindow, "Load new frame");
    loadNewImgBtn->setCallback([imageTexId, image_2]() {
        glBindTexture(GL_TEXTURE_2D, imageTexId);
        glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, image_2.cols, image_2.rows, GL_BGR, GL_UNSIGNED_BYTE, image_2.ptr());
    });

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

    screen->performLayout();
    screen->drawAll();
    screen->setVisible(true);
    // blitframebuffer
    nanogui::mainloop();
    nanogui::shutdown();

    return 0;
}