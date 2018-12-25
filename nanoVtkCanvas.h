
//
// Created by Max on 20.12.18.
//

#ifndef VISUALIZATION_NANOVTKCANVAS_H
#define VISUALIZATION_NANOVTKCANVAS_H

#endif //VISUALIZATION_NANOVTKCANVAS_H

#include <nanogui/widget.h>
#include <nanogui/glcanvas.h>
#include <nanogui/opengl.h>
#include <nanogui/glutil.h>


#include <vtkPolyDataMapper.h>
#include <vtkCommand.h>
#include <ExternalVTKWidget.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkLight.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>

#include <Eigen/Core>

using Vector3f = Eigen::Vector3f;

class NanoVtkCanvas : public nanogui::GLCanvas {
public:
    NanoVtkCanvas(Widget *parent) : nanogui::GLCanvas(parent) {
        if (!m_initialized) {
            glClearColor(0.2,0.1,0.3,1);
            glClear(GL_COLOR_BUFFER_BIT);

            GLfloat vertices[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                0.0f, 1.0f, 0.0f
            };

            glGenBuffers(1, &m_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


            vtkNew<vtkExternalOpenGLRenderWindow> renWin;

            m_externalVTKWidget->SetRenderWindow(renWin);

            vtkNew<vtkPolyDataMapper> mapper;
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);

            m_ren = m_externalVTKWidget->AddRenderer();
            m_ren->AddActor(actor);

            vtkNew<vtkCubeSource> cs;
            mapper->SetInputConnection(cs->GetOutputPort());
            actor->RotateX(45.0);
            actor->RotateY(45.0);

            m_ren->MakeCamera();
            m_ren->ResetCamera();

            m_rawWindow = parent->screen()->glfwWindow();
            m_initialized = true;
        }

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

        MatrixXu indices(3, 12); /* Draw a cube */
        indices.col( 0) << 0, 1, 3;
        indices.col( 1) << 3, 2, 1;
        indices.col( 2) << 3, 2, 6;
        indices.col( 3) << 6, 7, 3;
        indices.col( 4) << 7, 6, 5;
        indices.col( 5) << 5, 4, 7;
        indices.col( 6) << 4, 5, 1;
        indices.col( 7) << 1, 0, 4;
        indices.col( 8) << 4, 0, 3;
        indices.col( 9) << 3, 7, 4;
        indices.col(10) << 5, 6, 2;
        indices.col(11) << 2, 1, 5;

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
        mShader.uploadIndices(indices);

        mShader.uploadAttrib("position", positions);
        mShader.uploadAttrib("color", colors);
    }

    ~NanoVtkCanvas() {
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
        mShader.drawIndexed(GL_TRIANGLES, 0, 12);
        glDisable(GL_DEPTH_TEST);

        // m_externalVTKWidget->GetRenderWindow()->Start();
        // m_externalVTKWidget->GetRenderWindow()->Render();
        // return;

        // // Vector3f vertices[3];
        // // Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
        // // Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
        // // Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

        // // glEnable(GL_DEPTH_TEST);

        // glEnableVertexAttribArray(0);

        // glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDisableVertexAttribArray(0);

        // // glDisable(GL_DEPTH_TEST);
    }

private:
    nanogui::GLShader mShader;
    Eigen::Vector3f mRotation;
    bool m_initialized;
    GLFWwindow *m_rawWindow;
    vtkRenderer* m_ren;
    GLuint m_VBO;
    vtkNew<ExternalVTKWidget> m_externalVTKWidget;
};