
//
// Created by Max on 20.12.18.
//

#pragma once

#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <nanogui/opengl.h>
#include <nanogui/widget.h>

class TrajectoryView : public nanogui::GLCanvas {
public:
  TrajectoryView(Widget *parent) : nanogui::GLCanvas(parent) {
    using namespace nanogui;

    m_trajShader.init(
        /* An identifying name */
        "trajectory_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 modelViewProj;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(0.8, 0.0, 0.0, 0.8);\n"
        "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
        "}",

        /* Fragment shader */
        "#version 330\n"
        "out vec4 color;\n"
        "in vec4 frag_color;\n"
        "void main() {\n"
        "    color = frag_color;\n"
        "}");

    m_positions = MatrixXf(3, 4);

    m_positions.col(0) << -1, 1, 1;
    m_positions.col(1) << -1, 1, -1;
    m_positions.col(2) << 1, 1, -1;
    m_positions.col(3) << 1, 1, 1;

    m_trajShader.bind();
    m_trajShader.uploadAttrib("position", m_positions);

    m_camSymShader.init(
        /* An identifying name */
        "camera_symbol_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 modelViewProj;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(0.5, 0.5, 1.0, 1.0);\n"
        "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
        "}",

        /* Fragment shader */
        "#version 330\n"
        "out vec4 color;\n"
        "in vec4 frag_color;\n"
        "void main() {\n"
        "    color = frag_color;\n"
        "}");

    m_camSymShader.bind();
    m_camSymShader.uploadAttrib("position", m_positions);
  }

  ~TrajectoryView() {
    m_trajShader.free();
    m_camSymShader.free();
}

  void setRotation(nanogui::Vector3f vRotation) { mRotation = vRotation; }

  std::unique_ptr<nanogui::Vector3f> getLastPoint() {
    return std::make_unique<nanogui::Vector3f>(
        m_positions.col(m_positions.cols() - 1)
        );
  }

  void addPoint(nanogui::Vector3f newPoint) {
    m_positions.conservativeResize(Eigen::NoChange, m_positions.cols() + 1);
    m_positions.col(m_positions.cols() - 1) = newPoint;
    
    m_trajShader.bind();
    m_trajShader.uploadAttrib("position", m_positions);

    m_camSymShader.bind();
    m_camSymShader.uploadAttrib("position", m_positions);
  }

  virtual void drawGL() override {
    using namespace nanogui;

    Matrix4f mvp;
    mvp.setIdentity();
    float fTime = (float)glfwGetTime();
    mvp.topLeftCorner<3, 3>() =
        Eigen::Matrix3f(
            Eigen::AngleAxisf(mRotation[0] * fTime, Vector3f::UnitX()) *
            Eigen::AngleAxisf(mRotation[1] * fTime, Vector3f::UnitY()) *
            Eigen::AngleAxisf(mRotation[2] * fTime, Vector3f::UnitZ())) *
        0.25f;

    m_trajShader.bind();
    m_trajShader.setUniform("modelViewProj", mvp);

    glEnable(GL_DEPTH_TEST);
    m_trajShader.drawArray(GL_LINE_STRIP, 0, m_positions.cols() - 1);
    glDisable(GL_DEPTH_TEST);


    m_camSymShader.bind();
    m_camSymShader.setUniform("modelViewProj", mvp);

    glEnable(GL_DEPTH_TEST);
    m_camSymShader.drawArray(GL_LINE_STRIP, 0, m_positions.cols() - 1);
    glDisable(GL_DEPTH_TEST);
  }

private:
  nanogui::MatrixXf m_positions;
  nanogui::GLShader m_trajShader;
  nanogui::GLShader m_camSymShader;
  Eigen::Vector3f mRotation;
};