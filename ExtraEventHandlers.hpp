/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#pragma once

#include "Widget.hpp"
#include <vector>

START_NAMESPACE_DGL

static float clamp(float x, float upper, float lower)
{
    return std::min(upper, std::max(x, lower));
}

// --------------------------------------------------------------------------------------------------------------------

class SwitchEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void switchClicked(SubWidget *widget, bool down) = 0;
    };

    explicit SwitchEventHandler(SubWidget *self);
    explicit SwitchEventHandler(SubWidget *self, const SwitchEventHandler &other);
    SwitchEventHandler &operator=(const SwitchEventHandler &other);
    ~SwitchEventHandler();

    bool isDown() const noexcept;
    void setDown(bool down) noexcept;

    void setCallback(Callback *callback) noexcept;
    bool mouseEvent(const Widget::MouseEvent &ev);

protected:
private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(SwitchEventHandler)
};

class SliderEventHandler
{
public:
    enum Orientation
    {
        Horizontal,
        Vertical
    };

    // NOTE hover not implemented yet
    enum State
    {
        kSliderStateDefault = 0x0,
        kSliderStateHover = 0x1,
        kSliderStateDragging = 0x2,
        kSliderStateDraggingHover = kSliderStateDragging | kSliderStateHover
    };

    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void sliderDragStarted(SubWidget *widget) = 0;
        virtual void sliderDragFinished(SubWidget *widget) = 0;
        virtual void sliderValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit SliderEventHandler(SubWidget *self);
    explicit SliderEventHandler(SubWidget *self, const SliderEventHandler &other);
    SliderEventHandler &operator=(const SliderEventHandler &other);
    ~SliderEventHandler();

    // returns raw value, is assumed to be scaled if using log
    float getValue() const noexcept;

    // NOTE: value is assumed to be scaled if using log
    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    // returns 0-1 ranged value, already with log scale as needed
    float getNormalizedValue() const noexcept;

    // NOTE: value is assumed to be scaled if using log
    void setDefault(float def) noexcept;
    void setSliderArea(const double x, const double y, const double w, const double h) noexcept;
    void setInverted(bool inverted) noexcept;
    bool isInverted() noexcept;
    void setRange(float min, float max) noexcept;
    void setStep(float step) noexcept;
    void setUsingLogScale(bool yesNo) noexcept;
    void setStartPos(const int x, const int y) noexcept;
    void setEndPos(const int x, const int y) noexcept;
    void setCallback(Callback *callback) noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);

protected:
    // State getState() const noexcept;

private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(SliderEventHandler)
};

class SpinnerEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void spinnerValueChanged(SubWidget *widget, float value) = 0;
    };

    explicit SpinnerEventHandler(SubWidget *self);
    explicit SpinnerEventHandler(SubWidget *self, const SpinnerEventHandler &other);
    SpinnerEventHandler &operator=(const SpinnerEventHandler &other);
    ~SpinnerEventHandler();

    float getValue() const noexcept;

    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    void setIncrementArea(const double x, const double y, const double w, const double h) noexcept;
    void setDecrementArea(const double x, const double y, const double w, const double h) noexcept;
    void setRange(float min, float max) noexcept;
    void setStep(float step) noexcept;
    void setCallback(Callback *callback) noexcept;

    Rectangle<double> getIncrementArea() noexcept;
    Rectangle<double> getDecrementArea() noexcept;

    bool mouseEvent(const Widget::MouseEvent &ev);
    bool motionEvent(const Widget::MotionEvent &ev);
    bool scrollEvent(const Widget::ScrollEvent &ev);

protected:
    // State getState() const noexcept;

private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(SpinnerEventHandler)
};
// --------------------------------------------------------------------------------------------------------------------
class RadioEventHandler
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void radioValueChanged(SubWidget *widget, float value) = 0;
    };

    struct Option
    {
        const char *name;
        float value;
        Rectangle<double> hitbox;
  
        Option(const char *nm, float val)
        {
            name = nm;
            value = val;
        }
    };

    explicit RadioEventHandler(SubWidget *self);
    explicit RadioEventHandler(SubWidget *self, const RadioEventHandler &other);
    RadioEventHandler &operator=(const RadioEventHandler &other);
    ~RadioEventHandler();

    float getValue() const noexcept;

    virtual bool setValue(float value, bool sendCallback = false) noexcept;

    /*
     * addOption also calculates the hitboxes
    */
    void addOption(const char *name, float value);

    /* 
     * call this after changing size 
    */
    void initHitboxes();

    /*
     * clear the vector !
    */
    void getHitboxes(std::vector<Rectangle<double>> &hitboxes);
    void getOptions(std::vector<Option>&options);

    void setCallback(Callback *callback) noexcept;
    bool mouseEvent(const Widget::MouseEvent &ev);

protected:
    // State getState() const noexcept;

private:
    struct PrivateData;
    PrivateData *const pData;

    DISTRHO_LEAK_DETECTOR(SpinnerEventHandler)
};
// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
