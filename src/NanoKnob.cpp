/*
 * Copyright (C) 2021 Rob van den Berg <rghvdberg at gmail dot com>
 * SPDX-License-Identifier: ISC
*/

#include "NanoKnob.hpp"

START_NAMESPACE_DGL

NanoKnob::NanoKnob(Widget *const parent, KnobEventHandler::Callback *const cb)
    : NanoWidget(parent),
      KnobEventHandler(this)
{
    KnobEventHandler::setCallback(cb);
}

bool NanoKnob::onMouse(const MouseEvent &ev)
{
    return KnobEventHandler::mouseEvent(ev);
}

bool NanoKnob::onMotion(const MotionEvent &ev)
{
    return KnobEventHandler::motionEvent(ev);
}

END_NAMESPACE_DGL
