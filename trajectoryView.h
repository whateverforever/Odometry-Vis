
//
// Created by Max on 20.12.18.
//

#ifndef VISUALIZATION_TRAJECTORYVIEW_H
#define VISUALIZATION_TRAJECTORYVIEW_H

#endif //VISUALIZATION_TRAJECTORYVIEW_H

#include <nanogui/widget.h>
#include <nanogui/glcanvas.h>
#include <nanogui/opengl.h>
#include <nanogui/glutil.h>

class TrajectoryView : public nanogui::GLCanvas {
public:
    TrajectoryView(Widget *parent) : nanogui::GLCanvas(parent) {
        using namespace nanogui;

        mShader.init(
            /* An identifying name */
            "a_simple_shader",

            /* Vertex shader */
            "#version 330\n"
            "uniform mat4 modelViewProj;\n"
            "in vec3 position;\n"
            "in vec3 color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "    frag_color = 3.0 * modelViewProj * vec4(color, 1.0);\n"
            "    gl_Position = modelViewProj * vec4(position, 1.0);\n"
            "}",

            /* Fragment shader */
            "#version 330\n"
            "out vec4 color;\n"
            "in vec4 frag_color;\n"
            "void main() {\n"
            "    color = frag_color;\n"
            "}"
        );

        MatrixXf positions(3, 8);
        positions.col(0) << -1,  1,  1;
        positions.col(1) << -1,  1, -1;
        positions.col(2) <<  1,  1, -1;
        positions.col(3) <<  1,  1,  1;
        positions.col(4) << -1, -1,  1;
        positions.col(5) << -1, -1, -1;
        positions.col(6) <<  1, -1, -1;
        positions.col(7) <<  1, -1,  1;

        MatrixXf colors(3, 12);
        colors.col( 0) << 1, 0, 0;
        colors.col( 1) << 0, 1, 0;
        colors.col( 2) << 1, 1, 0;
        colors.col( 3) << 0, 0, 1;
        colors.col( 4) << 1, 0, 1;
        colors.col( 5) << 0, 1, 1;
        colors.col( 6) << 1, 1, 1;
        colors.col( 7) << 0.5, 0.5, 0.5;
        colors.col( 8) << 1, 0, 0.5;
        colors.col( 9) << 1, 0.5, 0;
        colors.col(10) << 0.5, 1, 0;
        colors.col(11) << 0.5, 1, 0.5;

        mShader.bind();

        mShader.uploadAttrib("position", positions);
        mShader.uploadAttrib("color", colors);
    }

    ~TrajectoryView() {
        mShader.free();
    }

    void setRotation(nanogui::Vector3f vRotation) {
        mRotation = vRotation;
    }

    virtual void drawGL() override {
        using namespace nanogui;

        mShader.bind();

        Matrix4f mvp;
        mvp.setIdentity();
        float fTime = (float)glfwGetTime();
        mvp.topLeftCorner<3,3>() = Eigen::Matrix3f(Eigen::AngleAxisf(mRotation[0]*fTime, Vector3f::UnitX()) *
                                                   Eigen::AngleAxisf(mRotation[1]*fTime,  Vector3f::UnitY()) *
                                                   Eigen::AngleAxisf(mRotation[2]*fTime, Vector3f::UnitZ())) * 0.25f;

        mShader.setUniform("modelViewProj", mvp);

        glEnable(GL_DEPTH_TEST);
        /* Draw 12 triangles starting at index 0 */
        mShader.drawArray(GL_LINE_STRIP, 0, 36);
        glDisable(GL_DEPTH_TEST);
    }

private:
    nanogui::GLShader mShader;
    Eigen::Vector3f mRotation;
};