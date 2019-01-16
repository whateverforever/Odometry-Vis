#include <chrono>
#include <functional>
#include <iostream>
#include <stdlib.h>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "Vis.h"

int main(int /* argc */, char ** /* argv */) {
  // Lib code!
  auto myUI = new Vis();
  auto dataGenerator = new DataGenerator();

  // Simulates the threads from Liu & Yu
  std::thread dataThread([dataGenerator, myUI](){
    
    while (true) {
      // img_l, img_r = camera->getPicture();
      // depth_l = stereo->getDepth(img_l, img_r);

      auto latestKeyframe = dataGenerator->getLatestKeyframe();

      myUI->loadNewestKeyframe(latestKeyframe);

      // auto latestCamPosition = latestCamPose.block<3,1>(0,3);
      // auto latestPoint = dataGenerator->getLatestPoint();
      // myUI->addTrajectoryPoint(latestPoint);

      std::this_thread::sleep_for(std::chrono::milliseconds(300)); // 30ms is ca 33fps
    }

  });

  myUI->initUI();
  myUI->startUI();

  dataThread.join();

  return 0;
}