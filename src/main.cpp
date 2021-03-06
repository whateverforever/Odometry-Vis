#include <chrono>
#include <functional>
#include <iostream>
#include <stdlib.h>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <DataGen.h>
#include <MockCameraInput.h>
#include <Vis.h>

int main(int /* argc */, char ** /* argv */) {
  // parameters of the freiburg camera
  float fx = 535.4, fy = 539.2, f_theta = 0, cx = 320.1, cy = 247.6;

  auto myUI = new Vis(fx, fy, f_theta, cx, cy);
  auto dataGenerator = new DataGenerator();
  auto mockCamera = new MockCameraInput();

  std::thread dataThread([mockCamera, dataGenerator, myUI]() {
    while (true) {
      // Getting the image from the camera, calculating the depth etc all
      // happens here dataGenerator is basically the last link in the chain:
      // Odometry auto latestKeyframe = dataGenerator->getLatestKeyframe();
      auto latestKeyframe = mockCamera->getLatestKeyframe();

      myUI->loadNewestKeyframe(latestKeyframe);

      std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
  });

  myUI->start();

  dataThread.join();

  delete myUI;
  delete dataGenerator;
  delete mockCamera;

  return 0;
}
