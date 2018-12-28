#pragma once

#include <opencv2/opencv.hpp>

#include "Vis.h"

class DataGenerator {
public:
    DataGenerator();
    void updateValue();
    friend class Vis;
private:
    int m_someValue;
    cv::Mat *m_activeImage;
    cv::Mat m_image_1;
    cv::Mat m_image_2;
};

DataGenerator::DataGenerator() {
    m_someValue = 555;

    m_image_1 = cv::imread("../data/rgbd_dataset_freiburg3_teddy/rgb/1341841873.273798.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    m_image_2 = cv::imread("../data/rgbd_dataset_freiburg3_teddy/rgb/1341841879.367159.png", CV_LOAD_IMAGE_COLOR);   // Read the file

    m_activeImage = &m_image_1;
}

void DataGenerator::updateValue() {
    m_someValue += 10;

    if(m_activeImage == &m_image_1) {
        m_activeImage = &m_image_1;
    } else {
        m_activeImage = &m_image_2;
    }
}