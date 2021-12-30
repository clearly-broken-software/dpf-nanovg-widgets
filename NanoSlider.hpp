/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"

START_NAMESPACE_DGL

class NanoSlider : public NanoSubWidget,
                   public SliderEventHandler
{
public:
    explicit NanoSlider(Widget *parent, SliderEventHandler::Callback *cb);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

private:
    DISTRHO_LEAK_DETECTOR(NanoSlider)
};

END_NAMESPACE_DGL
