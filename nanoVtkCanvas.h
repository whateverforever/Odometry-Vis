
//
// Created by Max on 20.12.18.
//

#ifndef VISUALIZATION_NANOVTKCANVAS_H
#define VISUALIZATION_NANOVTKCANVAS_H

#endif //VISUALIZATION_NANOVTKCANVAS_H

#include <nanogui/widget.h>
#include <nanogui/glcanvas.h>

class NanoVtkCanvas : public nanogui::GLCanvas {
public:
    NanoVtkCanvas(Widget *parent) : nanogui::GLCanvas(parent) {

    }

    virtual void drawGL() override {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
    }
};