#include <chrono>
#include <functional>
#include <iostream>
#include <stdlib.h>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "Vis.h"

int main(int /* argc */, char ** /* argv */) {
  auto myUI = new Vis();
  auto dataGenerator = new DataGenerator();

  std::thread dataThread([dataGenerator, myUI]() {
    while (true) {
      // Getting the image from the camera, calculating the depth etc all happens here
      // dataGenerator is basically the last link in the chain: Odometry
      auto latestKeyframe = dataGenerator->getLatestKeyframe();

      myUI->loadNewestKeyframe(latestKeyframe);

      std::this_thread::sleep_for(
          std::chrono::milliseconds(3000));
    }
  });

  myUI->start();

  dataThread.join();

  return 0;
}