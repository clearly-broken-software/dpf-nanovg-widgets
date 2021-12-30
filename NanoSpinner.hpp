/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"

START_NAMESPACE_DGL

class NanoSpinner : public NanoSubWidget,
                    public SpinnerEventHandler
{
public:
    explicit NanoSpinner(Widget *parent, SpinnerEventHandler::Callback *cb);

protected:
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;
    bool onScroll(const ScrollEvent &ev) override;

private:
    DISTRHO_LEAK_DETECTOR(NanoSpinner)
};

END_NAMESPACE_DGL
