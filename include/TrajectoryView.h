
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

    m_trajPositions = MatrixXf::Zero(3, 1);
    m_points = MatrixXf::Zero(3, 0);
    m_cameraLines = MatrixXf::Zero(3, 16);

    m_orthoZoom = 1;

    m_pan = {0, 0};
    m_rotate = {0, 0};

    m_trajShader.init(
        /* An identifying name */
        "trajectory_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projMatrix;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(0.7, 0.7, 0.7, 1.0);\n"
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

    m_camSymShader.init(
        /* An identifying name */
        "camera_symbol_shader",

        /* Vertex shader */
        "#version 330\n"
        "uniform mat4 viewMatrix;\n"
        "uniform mat4 projMatrix;\n"
        "in vec3 position;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "    frag_color = vec4(1.0, 1.0, 1.0, 1.0);\n"
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

    m_pointsShader.init("projected_points_shader",
                        /* Vertex shader */
                        "#version 330\n"
                        "uniform mat4 viewMatrix;\n"
                        "uniform mat4 projMatrix;\n"
                        "in vec3 position;\n"
                        "out vec4 frag_color;\n"
                        "void main() {\n"
                        "    gl_PointSize = 1.0;\n"
                        "    frag_color = vec4(1.0, 1.0, 1.0, 0.35);\n"
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

    m_gridShader.init("coordinate_grid_shader",
                      /* Vertex shader */
                      "#version 330\n"
                      "uniform mat4 viewMatrix;\n"
                      "uniform mat4 projMatrix;\n"
                      "in vec3 position;\n"
                      "out vec4 frag_color;\n"
                      "void main() {\n"
                      "    frag_color = vec4(position.x, 0, position.z, 1.0);\n"
                      "    gl_Position = projMatrix * viewMatrix *"
                      "vec4(position, 1.0);\n"
                      "}",

                      /* Fragment shader */
                      "#version 330\n"
                      "out vec4 color;\n"
                      "in vec4 frag_color;\n"
                      "void main() {\n"
                      "    color = frag_color;\n"
                      "}");

    int nLines = 21;
    float gridSize = 20;
    float offset = gridSize / (float)(nLines - 1);

    m_gridLines = MatrixXf::Zero(3, 2 * 2 * nLines);

    for (int i = 0; i < nLines; i++) {
      float x = i * offset - gridSize / 2;
      float z = i * offset - gridSize / 2;

      m_gridLines.col(4 * i + 0) = Vector3f(x, 0, -gridSize / 2);
      m_gridLines.col(4 * i + 1) = Vector3f(x, 0, gridSize / 2);

      m_gridLines.col(4 * i + 2) = Vector3f(-gridSize / 2, 0, z);
      m_gridLines.col(4 * i + 3) = Vector3f(gridSize / 2, 0, z);
    }

    m_gridShader.bind();
    m_gridShader.uploadAttrib("position", m_gridLines);

    m_shaders = {m_trajShader, m_camSymShader, m_gridShader, m_pointsShader};

    std::cout << "Grid lines:\n" << m_gridLines << std::endl;
  }

  bool mouseDragEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel,
                      int button, int modifiers) override {
    std::cout << "Dragging:" << p << ", rel: " << rel << ", button:" << button
              << ", modifiers:" << modifiers << std::endl;

    if (modifiers == 2) {
      m_pan += rel;
      return true;
    }

    m_rotate.x() += rel.x();
    return true;
  }

  bool scrollEvent(const nanogui::Vector2i &p,
                   const nanogui::Vector2f &rel) override {
    float scrollFactor = 0.1;

    m_orthoZoom += scrollFactor * rel.y();

    if (m_orthoZoom < 0) {
      m_orthoZoom = 0;
    }

    return true;
  }

  ~TrajectoryView() {
    for (auto shader : m_shaders) {
      shader.free();
    }
  }

  void setRotation(nanogui::Vector3f vRotation) { m_Rotation = vRotation; }

  void setZoom(float zoom) {
    std::cout << "Increased zoom:" << zoom << std::endl;
    m_orthoZoom = zoom;
  }

  float getZoom() { return m_orthoZoom; }

  std::unique_ptr<nanogui::Vector3f> getLastPoint() {
    return std::make_unique<nanogui::Vector3f>(
        m_trajPositions.col(m_trajPositions.cols() - 1));
  }

  // TODO: rename or sth to make clear it's about trajectory points
  void addPoint(const nanogui::Vector3f &point) {
    m_trajPositions.conservativeResize(Eigen::NoChange,
                                       m_trajPositions.cols() + 1);
    m_trajPositions.col(m_trajPositions.cols() - 1) = point;
  }

  void addPoints(const std::vector<nanogui::Vector3f> &newPoints) {
    int numExistingPoints = m_points.cols();
    int numNewPoints = newPoints.size();

    m_points.conservativeResize(Eigen::NoChange,
                                numExistingPoints + numNewPoints);

    for (int i = 0; i < numNewPoints; i++) {
      m_points.col(numExistingPoints + i) = newPoints[i];
    }

    m_pointsShader.bind();
    m_pointsShader.uploadAttrib("position", m_points);

    // std::cout << "Added new bunch of points:\n" << m_points << std::endl;
  }

  void addPose(odometry::Affine4f pose) {
    nanogui::Matrix3f newRotation = pose.block<3, 3>(0, 0);
    nanogui::Vector3f newPoint = pose.block<3, 1>(0, 3);

    m_trajPositions.conservativeResize(Eigen::NoChange,
                                       m_trajPositions.cols() + 1);
    m_trajPositions.col(m_trajPositions.cols() - 1) = newPoint;

    m_trajShader.bind();
    m_trajShader.uploadAttrib("position", m_trajPositions);

    auto newCamVerts = nanogui::MatrixXf(3, 16);
    auto l = 0.1;

    // clang-format off
    newCamVerts <<  0,   l,   0,   l,   0,  -l,   0,  -l,   l,   l,   l,  -l,  -l,  -l,  -l,   l,
                    0,  -l,   0,   l,   0,  -l,   0,   l,  -l,   l,  -l,  -l,  -l,   l,   l,   l,
                    0, 2*l,   0, 2*l,   0, 2*l,   0, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l, 2*l;
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
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float fTime = (float)glfwGetTime();

    Matrix4f modelMatrix;
    modelMatrix.setIdentity();
    modelMatrix.topLeftCorner<3, 3>() =
        Eigen::Matrix3f(
            Eigen::AngleAxisf(0, Vector3f::UnitX()) *
            Eigen::AngleAxisf(m_Rotation[1] * 0.5 * fTime, Vector3f::UnitY()) *
            Eigen::AngleAxisf(0, Vector3f::UnitZ())) *
        0.25f;

    Vector3f sceneMaxima = m_trajPositions.rowwise().maxCoeff();
    Vector3f sceneMinima = m_trajPositions.rowwise().minCoeff();
    Vector3f sceneMean = m_trajPositions.rowwise().mean();
    Vector3f sceneCenter = (sceneMaxima + sceneMinima) / 2;

    // addPoint(sceneCenter);

    Vector3f sceneBB = (sceneMaxima - sceneMinima).cwiseAbs();

    float bSphereRadius = sceneBB.norm() / 2;
    float padding = 1.05; // add %5 padding around the scene
    bSphereRadius *= padding;

    float aspectRatio_screen = (float)this->width() / (float)this->height();
    float l, r, t, b, f, n;

    l = -bSphereRadius * aspectRatio_screen / m_orthoZoom;
    r = bSphereRadius * aspectRatio_screen / m_orthoZoom;

    b = -bSphereRadius / m_orthoZoom;
    t = bSphereRadius / m_orthoZoom;

    Vector3f cameraPos = {20, 20, 20}; // sceneMaxima;

    n = -10;
    f = 100;

    Matrix4f projMatrix;
    projMatrix = nanogui::ortho(l, r, b, t, n, f);

    Matrix4f viewMatrix;
    viewMatrix = Matrix4f::Identity();
    viewMatrix *= nanogui::lookAt(cameraPos, sceneCenter, Vector3f(0, 1, 0));

    float panFactor = 2;
    Vector3f panMatrix = nanogui::unproject(
        {(float)m_pan.x() * panFactor / m_orthoZoom,
         -(float)m_pan.y() * panFactor / m_orthoZoom, 0},
        Matrix4f::Identity(), viewMatrix, {this->width(), this->height()});

    viewMatrix *= nanogui::translate(panMatrix);

    float rotateFactor = 0.01;
    Matrix3f m = Eigen::Matrix3f(
        Eigen::AngleAxisf(0, Vector3f::UnitX()) *
        Eigen::AngleAxisf(m_rotate.x() * rotateFactor, Vector3f::UnitY()) *
        Eigen::AngleAxisf(0, Vector3f::UnitZ()));

    Matrix4f m2 = Matrix4f::Identity();
    m2.block<3, 3>(0, 0) = m;

    viewMatrix *= m2;

    for (auto shader : m_shaders) {
      shader.bind();
      shader.setUniform("viewMatrix", viewMatrix);
      shader.setUniform("projMatrix", projMatrix);
    }

    m_trajShader.bind();
    m_trajShader.drawArray(GL_LINE_STRIP, 0, m_trajPositions.cols());

    m_camSymShader.bind();
    m_camSymShader.drawArray(GL_LINES, 0, m_cameraLines.cols());

    m_gridShader.bind();
    m_gridShader.drawArray(GL_LINES, 0, m_gridLines.cols());

    m_pointsShader.bind();
    m_pointsShader.drawArray(GL_POINTS, 0, m_points.cols());

    glDisable(GL_BLEND);
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_DEPTH_TEST);
  }

private:
  nanogui::MatrixXf m_trajPositions;
  nanogui::MatrixXf m_cameraLines;
  nanogui::MatrixXf m_gridLines;
  nanogui::MatrixXf m_points;

  nanogui::GLShader m_trajShader;
  nanogui::GLShader m_camSymShader;
  nanogui::GLShader m_gridShader;
  nanogui::GLShader m_pointsShader;

  std::vector<nanogui::GLShader> m_shaders;

  Eigen::Vector3f m_Rotation;

  nanogui::Vector2i m_mousePrevious;
  nanogui::Vector2i m_pan;
  nanogui::Vector2i m_rotate;

  float m_orthoZoom;
};
