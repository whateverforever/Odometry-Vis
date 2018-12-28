//
// Created by Max on 28.12.18.
//

#pragma once

#include <functional>
#include <nanogui/nanogui.h>

class VisScreen : public nanogui::Screen {

public:
    VisScreen(const nanogui::Vector2i &size, const std::string &caption) : nanogui::Screen(size, caption){};
    void onUpdate(const std::function<void()> &callback);
    void drawContents() override { m_update_callback(); };

private:
    std::function<void()> m_update_callback;
};

