
//
// Created by Max on 20.12.18.
//

#ifndef VISUALIZATION_NANOVTKCANVAS_H
#define VISUALIZATION_NANOVTKCANVAS_H

#endif //VISUALIZATION_NANOVTKCANVAS_H

#include <nanogui/widget.h>
#include <nanogui/glcanvas.h>
#include <nanogui/opengl.h>

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
            vtkNew<vtkExternalOpenGLRenderWindow> renWin;

            m_externalVTKWidget->SetRenderWindow(renWin);

            vtkNew<vtkPolyDataMapper> mapper;
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);

            vtkRenderer* ren = m_externalVTKWidget->AddRenderer();
            ren->AddActor(actor);

            vtkNew<vtkCubeSource> cs;
            mapper->SetInputConnection(cs->GetOutputPort());
            actor->RotateX(45.0);
            actor->RotateY(45.0);
            ren->ResetCamera();

            m_rawWindow = parent->screen()->glfwWindow();
            m_initialized = true;
        }
    }

    virtual void drawGL() override {
        m_externalVTKWidget->GetRenderWindow()->Render();

        Vector3f Vertices[1];
        Vertices[0] = Vector3f(0.0f, 0.0f, 0.0f);

        GLuint VBO;
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_POINTS, 0, 1);

        glDisableVertexAttribArray(0);
    }

private:
    bool m_initialized;
    GLFWwindow *m_rawWindow;
    vtkNew<ExternalVTKWidget> m_externalVTKWidget;
};