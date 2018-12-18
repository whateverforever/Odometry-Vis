/*
    src/example2.cpp -- C++ version of an example application that shows
    how to use the form helper class. For a Python implementation, see
    '../python/example2.py'.
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.
    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <stdlib.h>

#include <nanogui/nanogui.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>

using namespace nanogui;

enum test_enum {
    Item1 = 0,
    Item2,
    Item3
};

bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);

int getTextureForMat(cv::Mat &mat) {
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
    cv::Mat image;
    image = cv::imread("./data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    cv::waitKey(0);

    // Second, display the image using nano
    nanogui::init();
    Screen *screen = new Screen(Vector2i(500, 700), "NanoGUI test");

    auto imageWindow = new Window(screen, "RGB Left");
    imageWindow->setPosition({50, 50});
    imageWindow->setLayout(new GroupLayout());

    int imageTexId = getTextureForMat(image);

    auto imageView = new ImageView(imageWindow, imageTexId);
    imageView->setSize({100,100});
    imageView->setScaleCentered(1);
    imageView->fit();

    screen->drawAll();
    screen->setVisible(true);
    imageWindow->center();

    nanogui::mainloop();
    nanogui::shutdown();

    return 0;
}