/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#include "NanoSwitch.hpp"

START_NAMESPACE_DGL

NanoSwitch::NanoSwitch(Widget *const parent, SwitchEventHandler::Callback *const cb)
    : NanoWidget(parent),
      SwitchEventHandler(this)
{
    SwitchEventHandler::setCallback(cb);
}

bool NanoSwitch::onMouse(const MouseEvent &ev)
{
    return SwitchEventHandler::mouseEvent(ev);
}

END_NAMESPACE_DGL
