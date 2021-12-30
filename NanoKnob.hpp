/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "EventHandlers.hpp"

START_NAMESPACE_DGL

class NanoKnob : public NanoSubWidget,
                 public KnobEventHandler
{
public:
    explicit NanoKnob(Widget *parent, KnobEventHandler::Callback *cb);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

private:
    DISTRHO_LEAK_DETECTOR(NanoKnob)
};

END_NAMESPACE_DGL
