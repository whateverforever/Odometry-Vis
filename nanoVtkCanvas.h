
//
// Created by Max on 20.12.18.
//

#ifndef VISUALIZATION_NANOVTKCANVAS_H
#define VISUALIZATION_NANOVTKCANVAS_H

#endif //VISUALIZATION_NANOVTKCANVAS_H

#include <nanogui/widget.h>
#include <nanogui/glcanvas.h>

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

class NanoVtkCanvas : public nanogui::GLCanvas {
public:
    NanoVtkCanvas(Widget *parent) : nanogui::GLCanvas(parent) {
        if (!m_initialized) {
            vtkNew<vtkExternalOpenGLRenderWindow> renWin;

            externalVTKWidget->SetRenderWindow(renWin);

            vtkNew<vtkPolyDataMapper> mapper;
            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);

            vtkRenderer* ren = externalVTKWidget->AddRenderer();
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
        //glEnable(GL_DEPTH_TEST);

        externalVTKWidget->GetRenderWindow()->Render();

        //glDisable(GL_DEPTH_TEST);
    }

private:
    bool m_initialized;
    GLFWwindow *m_rawWindow;
    vtkNew<ExternalVTKWidget> externalVTKWidget;
};