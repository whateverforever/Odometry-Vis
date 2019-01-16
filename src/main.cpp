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
      dataGenerator->updateValue();

      auto latestPoint = dataGenerator->getLatestPoint();

      myUI->addTrajectoryPoint(latestPoint);

      std::this_thread::sleep_for(std::chrono::milliseconds(30)); // 30ms is ca 33fps
    }

  });

  myUI->initUI();
  myUI->startUI();

  dataThread.join();

  return 0;
}