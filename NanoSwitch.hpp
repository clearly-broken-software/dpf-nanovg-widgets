/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"

START_NAMESPACE_DGL

class NanoSwitch : public NanoSubWidget,
                   public SwitchEventHandler
{
public:
    explicit NanoSwitch(Widget *parent, SwitchEventHandler::Callback *cb);

protected:
    bool onMouse(const MouseEvent &ev) override;

private:
    DISTRHO_LEAK_DETECTOR(NanoSwitch)
};

END_NAMESPACE_DGL
