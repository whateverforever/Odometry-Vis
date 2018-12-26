#include "main.h"

#include <stdlib.h>
#include <iostream>
#include <thread>
#include <functional>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "library.h"

void dataGenerator(Vis *visualization) {
    // 1. generate data
    // 2. pass message w/ data
    // 3. listen in UI to message, act
    std::cout << "From inside:" << visualization << std::endl;
    
    visualization->calledFromOutSide();
    visualization->calledFromOutSide();
    visualization->calledFromOutSide();
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

    // Lib code!
    auto visu = new Vis();

    std::cout << "From outside:" << visu << std::endl;

    std::thread dataThread(dataGenerator, std::ref(visu));

    visu->initUI();
    visu->startUI();

    dataThread.join();

    return 0;
}