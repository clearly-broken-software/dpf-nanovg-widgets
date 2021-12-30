/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#include "NanoSpinner.hpp"

START_NAMESPACE_DGL

NanoSpinner::NanoSpinner(Widget *const parent, SpinnerEventHandler::Callback *const cb)
    : NanoWidget(parent),
      SpinnerEventHandler(this)
{
    SpinnerEventHandler::setCallback(cb);
}

bool NanoSpinner::onMouse(const MouseEvent &ev)
{
    return SpinnerEventHandler::mouseEvent(ev);
}

bool NanoSpinner::onMotion(const MotionEvent &ev)
{
    return SpinnerEventHandler::motionEvent(ev);
}

bool NanoSpinner::onScroll(const ScrollEvent &ev)
{
    return SpinnerEventHandler::scrollEvent(ev);
}

END_NAMESPACE_DGL
