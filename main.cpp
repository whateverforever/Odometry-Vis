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

int main(int /* argc */, char ** /* argv */) {

    // First, load an image with openCV and do stuff with it
//    cv::Mat image;
//    image = cv::imread("./data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png", CV_LOAD_IMAGE_COLOR);   // Read the file
//
//    if(! image.data )                              // Check for invalid input
//    {
//        std::cout <<  "Could not open or find the image" << std::endl ;
//        return -1;
//    }
//
//    namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
//    imshow( "Display window", image );                   // Show our image inside it.
//
//    cv::waitKey(0);

    // Second, display the image using nano
    nanogui::init();
    Screen *screen = new Screen(Vector2i(500, 700), "NanoGUI test");

    auto imageWindow = new Window(screen, "Selected image");
    imageWindow->setPosition(Vector2i(0, 0));
    imageWindow->setLayout(new GroupLayout());

    GLuint imageTexId;
    glGenTextures(1, &imageTexId);
    glBindTexture(GL_TEXTURE_2D, imageTexId);

    int width = 48;
    int height = 48;

    float pixels[3*width*height];
    for (int i = 0; i < 3*width*height; i++) {
        pixels[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto imageView = new ImageView(imageWindow, imageTexId);
    imageView->setPosition(Vector2i(50,50));
    imageView->setGridThreshold(20);
    imageView->setPixelInfoThreshold(20);
    imageView->fit();

    screen->drawAll();
    screen->setVisible(true);

    nanogui::mainloop();
    nanogui::shutdown();

    return 0;

    nanogui::init();

    /* scoped variables */ {
        bool use_gl_4_1 = false;// Set to true to create an OpenGL 4.1 context.
        Screen *screen = nullptr;

        if (use_gl_4_1) {
            // NanoGUI presents many options for you to utilize at your discretion.
            // See include/nanogui/screen.h for what all of these represent.
            screen = new Screen(Vector2i(500, 700), "NanoGUI test [GL 4.1]",
                    /*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
                    /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                    /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
        } else {
            screen = new Screen(Vector2i(500, 700), "NanoGUI test");
        }

        bool enabled = true;
        FormHelper *gui = new FormHelper(screen);
        ref<Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
        gui->addGroup("Basic types");
        gui->addVariable("bool", bvar);
        gui->addVariable("string", strval);

        gui->addGroup("Validating fields");
        gui->addVariable("int", ivar)->setSpinnable(true);
        gui->addVariable("float", fvar);
        gui->addVariable("double", dvar)->setSpinnable(true);

        gui->addGroup("Complex types");
        gui->addVariable("Enumeration", enumval, enabled)
                ->setItems({"Item 1", "Item 2", "Item 3"});
        gui->addVariable("Color", colval)
                ->setFinalCallback([](const Color &c) {
                    std::cout << "ColorPicker Final Callback: ["
                              << c.r() << ", "
                              << c.g() << ", "
                              << c.b() << ", "
                              << c.w() << "]" << std::endl;
                });

        gui->addGroup("Other widgets");
        gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; });

        screen->setVisible(true);
        screen->performLayout();
        window->center();

        nanogui::mainloop();
    }

    nanogui::shutdown();
    return 0;
}