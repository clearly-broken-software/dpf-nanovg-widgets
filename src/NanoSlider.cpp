/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#include "NanoSlider.hpp"

START_NAMESPACE_DGL

NanoSlider::NanoSlider(Widget *const parent, SliderEventHandler::Callback *const cb)
    : NanoWidget(parent),
      SliderEventHandler(this)
{
    SliderEventHandler::setCallback(cb);
}

bool NanoSlider::onMouse(const MouseEvent &ev)
{
    return SliderEventHandler::mouseEvent(ev);
}

bool NanoSlider::onMotion(const MotionEvent &ev)
{
    return SliderEventHandler::motionEvent(ev);
}

END_NAMESPACE_DGL
