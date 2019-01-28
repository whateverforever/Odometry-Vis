
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

    m_orthoZoom = 4;

    m_trajShader.init(
        /* An identifying name */
        "trajectory_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projMatrix;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(0.7, 0.7, 0.7, 1.0);\n"
        "    gl_Position = projMatrix * viewMatrix * modelMatrix * "
        "vec4(position, 1.0);\n"
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
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projMatrix;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    gl_Position = projMatrix * viewMatrix * modelMatrix * "
        "vec4(position, 1.0);\n"
        "}",

        /* Fragment shader */
        "#version 330\n"
        "out vec4 color;\n"
        "in vec4 frag_color;\n"
        "void main() {\n"
        "    color = frag_color;\n"
        "}");

    m_gridShader.init("coordinate_grid_shader",
                      /* Vertex shader */
                      "#version 330\n"
                      "uniform mat4 modelMatrix;\n"
                      "uniform mat4 viewMatrix;\n"
                      "uniform mat4 projMatrix;\n"
                      "in vec3 position;\n"
                      "out vec4 frag_color;\n"
                      "void main() {\n"
                      "    frag_color = vec4(position.x, 0, position.z, 1.0);\n"
                      "    gl_Position = projMatrix * viewMatrix * "
                      "vec4(position, 1.0);\n"
                      "}",

                      /* Fragment shader */
                      "#version 330\n"
                      "out vec4 color;\n"
                      "in vec4 frag_color;\n"
                      "void main() {\n"
                      "    color = frag_color;\n"
                      "}");

    int nLines = 11;
    float gridSize = 1;
    float offset = gridSize / (float)(nLines-1);

    m_gridLines = MatrixXf::Zero(3, 2 * 2 * nLines);

    for (int i = 0; i < nLines; i ++) {
        float x = i*offset -gridSize/2;
        float z = i*offset -gridSize/2;

        m_gridLines.col(4*i + 0) = Vector3f(x, 0, -gridSize/2);
        m_gridLines.col(4*i + 1) = Vector3f(x, 0, gridSize/2);

        m_gridLines.col(4*i + 2) = Vector3f(-gridSize/2, 0, z);
        m_gridLines.col(4*i + 3) = Vector3f( gridSize/2, 0, z);
    }

    m_gridShader.bind();
    m_gridShader.uploadAttrib("position", m_gridLines);

    m_shaders = {
        m_trajShader,
        m_camSymShader,
        m_gridShader
    };

    std::cout << "Grid lines:\n" << m_gridLines << std::endl;
  }

  ~TrajectoryView() {
    m_trajShader.free();
    m_camSymShader.free();
  }

  void setRotation(nanogui::Vector3f vRotation) { m_Rotation = vRotation; }

  void setZoom(float zoom) {
    std::cout << "Increased zoom:" << zoom << std::endl;
    m_orthoZoom = zoom;
  }

  float getZoom() { return m_orthoZoom; }

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

    Matrix4f modelMatrix;
    modelMatrix.setIdentity();
    modelMatrix.topLeftCorner<3, 3>() =
        Eigen::Matrix3f(
            Eigen::AngleAxisf(0, Vector3f::UnitX()) *
            Eigen::AngleAxisf(m_Rotation[1] * 0.5 * fTime, Vector3f::UnitY()) *
            Eigen::AngleAxisf(0, Vector3f::UnitZ())) *
        0.25f;

    Vector3f sceneMaxima = m_positions.rowwise().maxCoeff();
    Vector3f sceneMinima = m_positions.rowwise().minCoeff();
    Vector3f sceneCenter = (sceneMaxima + sceneMinima) / 2;

    std::cout << "Scene Center:\n" << sceneCenter << std::endl;

    Vector3f sceneBB = (sceneMaxima - sceneMinima).cwiseAbs();
    float bSphereRadius = sceneBB.norm() / 2;

    float aspectRatio_screen = (float)this->width() / (float)this->height();


    float l, r, t, b, f, n;

    l = -bSphereRadius * aspectRatio_screen / m_orthoZoom;
    r =  bSphereRadius * aspectRatio_screen / m_orthoZoom;

    b = -bSphereRadius / m_orthoZoom;
    t =  bSphereRadius / m_orthoZoom;

    n = 0;
    f = 2 * bSphereRadius;

    Matrix4f projMatrix;
    projMatrix = nanogui::ortho(l, r, b, t, n, f);
    // projMatrix.transposeInPlace();

    Matrix4f viewMatrix =
        nanogui::lookAt(Vector3f(1, 1, 1), sceneCenter, Vector3f(0, 1, 0));

    for(auto shader : m_shaders) {
        shader.bind();
        shader.setUniform("modelMatrix", modelMatrix);
        shader.setUniform("viewMatrix", viewMatrix);
        shader.setUniform("projMatrix", projMatrix);
    }

    m_trajShader.bind();
    m_trajShader.drawArray(GL_LINE_STRIP, 0, m_positions.cols());

    m_camSymShader.bind();
    m_camSymShader.drawArray(GL_LINES, 0, m_cameraLines.cols());

    m_gridShader.bind();
    m_gridShader.drawArray(GL_LINES, 0, m_gridLines.cols());

    glDisable(GL_DEPTH_TEST);
  }

private:
  nanogui::MatrixXf m_positions;
  nanogui::MatrixXf m_cameraLines;
  nanogui::MatrixXf m_gridLines;

  nanogui::GLShader m_trajShader;
  nanogui::GLShader m_camSymShader;
  nanogui::GLShader m_gridShader;

  std::vector<nanogui::GLShader> m_shaders;

  Eigen::Vector3f m_Rotation;

  float m_orthoZoom;
};
