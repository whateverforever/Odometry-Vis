#pragma once

#include <iostream>
#include <map>
#include <string>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/nanogui.h>

#include <Keyframe.h>

#include "TrajectoryView.h"
#include "VisScreen.h"

class Vis {
public:
  Vis(float, float, float, float, float);
  void start();
  void loadNewestKeyframe(const odometry::KeyFrame &);

private:
  double m_lastFrameTime;
  int m_numElapsedFrames;
  double m_fps;

  GLuint m_rgbLeftTexId;
  GLuint m_rgbRightTexId;
  GLuint m_depthLeftTexId;

  nanogui::Matrix4f m_intrinsics;
  nanogui::Matrix4f m_intrinsicsInv;

  std::vector<odometry::KeyFrame> m_keyframeBuffer;
  TrajectoryView *m_view;
};
