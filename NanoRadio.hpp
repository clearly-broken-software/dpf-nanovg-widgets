/*
 * Copyright (C) 2022 Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "NanoVG.hpp"
#include "ExtraEventHandlers.hpp"

START_NAMESPACE_DGL

class NanoRadio : public NanoSubWidget, public RadioEventHandler
{
public:
    explicit NanoRadio(Widget *parent, RadioEventHandler::Callback *cb);

protected:
    bool onMouse(const MouseEvent &ev) override;

private:
    DISTRHO_LEAK_DETECTOR(NanoRadio)
};

END_NAMESPACE_DGL
