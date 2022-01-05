/*
 * Copyright (C) 2022 Rob van den Berg <rghvdberg at gmail dot com>
 * SPDX-License-Identifier: ISC
*/

#include "NanoRadio.hpp"

START_NAMESPACE_DGL

NanoRadio::NanoRadio(Widget *const parent, RadioEventHandler::Callback *cb)
    : NanoWidget(parent),
      RadioEventHandler(this)
{
    RadioEventHandler::setCallback(cb);
}

bool NanoRadio::onMouse(const MouseEvent &ev)
{
    return RadioEventHandler::mouseEvent(ev);
}

END_NAMESPACE_DISTRHO
