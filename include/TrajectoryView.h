
//
// Created by Max on 20.12.18.
//

#pragma once

#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <nanogui/opengl.h>
#include <nanogui/widget.h>

#include <Eigen/Geometry>

class TrajectoryView : public nanogui::GLCanvas {
public:
  TrajectoryView(Widget *parent) : nanogui::GLCanvas(parent) {
    using namespace nanogui;

    m_positions = MatrixXf::Zero(3, 1);
    m_cameraLines = MatrixXf::Zero(3, 16);

    m_trajShader.init(
        /* An identifying name */
        "trajectory_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 modelViewProj;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(0.7, 0.7, 0.7, 1.0);\n"
        "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
        "}",

        /* Fragment shader */
        "#version 330\n"
        "out vec4 color;\n"
        "in vec4 frag_color;\n"
        "void main() {\n"
        "    color = frag_color;\n"
        "}");

    m_camSymShader.init(
        /* An identifying name */
        "camera_symbol_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 modelViewProj;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
        "}",

        /* Fragment shader */
        "#version 330\n"
        "out vec4 color;\n"
        "in vec4 frag_color;\n"
        "void main() {\n"
        "    color = frag_color;\n"
        "}");
  }

  ~TrajectoryView() {
    m_trajShader.free();
    m_camSymShader.free();
  }

  void setRotation(nanogui::Vector3f vRotation) { mRotation = vRotation; }

  std::unique_ptr<nanogui::Vector3f> getLastPoint() {
    return std::make_unique<nanogui::Vector3f>(
        m_positions.col(m_positions.cols() - 1));
  }

  void addPose(odometry::Affine4f pose) {
    nanogui::Matrix3f newRotation = pose.block<3, 3>(0, 0);
    nanogui::Vector3f newPoint = pose.block<3, 1>(0, 3);

    m_positions.conservativeResize(Eigen::NoChange, m_positions.cols() + 1);
    m_positions.col(m_positions.cols() - 1) = newPoint;

    m_trajShader.bind();
    m_trajShader.uploadAttrib("position", m_positions);

    auto newCamVerts = nanogui::MatrixXf(3, 16);
    auto l = 0.35;

    // clang-format off
    newCamVerts <<  0,   l,   0,   l,   0,  -l,   0,  -l,   l,   l,   l,  -l,  -l,  -l,  -l,   l, 
                    0, 2*l,   0, 2*l,   0, 2*l,   0, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l,
                    0,  -l,   0,   l,   0,  -l,   0,   l,  -l,   l,  -l,  -l,  -l,   l,   l,   l;
    // clang-format on

    newCamVerts = newRotation * newCamVerts;
    newCamVerts.colwise() += newPoint;

    m_cameraLines.conservativeResize(Eigen::NoChange,
                                     m_cameraLines.cols() + 16);
    m_cameraLines.block<3, 16>(0, m_cameraLines.cols() - 16) = newCamVerts;

    m_camSymShader.bind();
    m_camSymShader.uploadAttrib("position", m_cameraLines);
  }

  virtual void drawGL() override {
    using namespace nanogui;
    glEnable(GL_DEPTH_TEST);

    float fTime = (float)glfwGetTime();

    Matrix4f mvp;
    mvp.setIdentity();
    mvp.topLeftCorner<3, 3>() =
        Eigen::Matrix3f(
            Eigen::AngleAxisf(mRotation[0] * 0.5 * fTime, Vector3f::UnitX()) *
            Eigen::AngleAxisf(mRotation[1] * 0.5 * fTime, Vector3f::UnitY()) *
            Eigen::AngleAxisf(mRotation[2] * 0.5 * fTime, Vector3f::UnitZ())) *
        0.25f;

    m_trajShader.bind();
    m_trajShader.setUniform("modelViewProj", mvp);
    m_trajShader.drawArray(GL_LINE_STRIP, 0, m_positions.cols());

    m_camSymShader.bind();
    m_camSymShader.setUniform("modelViewProj", mvp);
    m_camSymShader.drawArray(GL_LINES, 0, m_cameraLines.cols());

    glDisable(GL_DEPTH_TEST);
  }

private:
  nanogui::MatrixXf m_positions;
  nanogui::MatrixXf m_cameraLines;

  nanogui::GLShader m_trajShader;
  nanogui::GLShader m_camSymShader;
  Eigen::Vector3f mRotation;
};