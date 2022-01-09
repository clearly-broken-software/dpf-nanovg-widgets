/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#include "NanoButton.hpp"

START_NAMESPACE_DGL

NanoButton::NanoButton(Widget* const parent, ButtonEventHandler::Callback* const cb)
    : NanoWidget(parent),
      ButtonEventHandler(this)

{
    ButtonEventHandler::setCallback(cb);
}

bool NanoButton::onMouse(const MouseEvent& ev)
{
    return ButtonEventHandler::mouseEvent(ev);
}

END_NAMESPACE_DGL
