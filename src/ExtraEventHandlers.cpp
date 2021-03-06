/*
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>, Rob van den Berg <rghvdberg at gmail dot com
 * SPDX-License-Identifier: ISC
*/

#include "ExtraEventHandlers.hpp"
#include "SubWidget.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

struct SwitchEventHandler::PrivateData
{
    SwitchEventHandler *const self;
    SubWidget *const widget;
    SwitchEventHandler::Callback *callback;

    bool isDown;

    PrivateData(SwitchEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          isDown(false),
          callback(nullptr)
    {
    }

    PrivateData(SwitchEventHandler *const s, SubWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          isDown(other->isDown)

    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        isDown = other->isDown;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.press && widget->contains(ev.pos))
        {
            isDown = !isDown;
            widget->repaint();

            if (callback != nullptr)
            {
                try
                {
                    callback->switchClicked(widget, isDown);
                }
                DISTRHO_SAFE_EXCEPTION("SwitchEventHandler::mouseEvent");
            }

            return true;
        }

        return false;
    }

    void setDown(const bool down) noexcept
    {
        isDown = down;
        widget->repaint();
    }
};

// --------------------------------------------------------------------------------------------------------------------

SwitchEventHandler::SwitchEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

SwitchEventHandler::SwitchEventHandler(SubWidget *const self, const SwitchEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

SwitchEventHandler &SwitchEventHandler::operator=(const SwitchEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

SwitchEventHandler::~SwitchEventHandler()
{
    delete pData;
}

void SwitchEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool SwitchEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool SwitchEventHandler::isDown() const noexcept
{
    return pData->isDown;
}

void SwitchEventHandler::setDown(const bool down) noexcept
{
    return pData->setDown(down);
}

// --------------------------------------------------------------------------------------------------------------------

// begin slider
struct SliderEventHandler::PrivateData
{
    SliderEventHandler *const self;
    SubWidget *const widget;
    SliderEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float step;
    float value;
    float valueDef;
    float valueTmp;
    bool usingDefault;
    bool usingLog;
    bool dragging;
    bool inverted;
    bool valueIsSet;
    double startedX;
    double startedY;
    Point<int> startPos;
    Point<int> endPos;
    Rectangle<double> sliderArea;

    PrivateData(SliderEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          step(0.0f),
          value(0.5f),
          valueDef(value),
          valueTmp(value),
          usingDefault(false),
          usingLog(false),
          dragging(false),
          inverted(false),
          valueIsSet(false),
          startedX(0.0),
          startedY(0.0),
          startPos(),
          endPos(),
          sliderArea()
    {
    }

    PrivateData(SliderEventHandler *const s, SubWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          step(other->step),
          value(other->value),
          valueDef(other->valueDef),
          valueTmp(value),
          usingDefault(other->usingDefault),
          usingLog(other->usingDefault),
          startPos(other->startPos),
          endPos(other->endPos),
          dragging(false),
          inverted(other->inverted),
          valueIsSet(false),
          sliderArea(other->sliderArea)

    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        step = other->step;
        value = other->value;
        valueDef = other->valueDef;
        valueTmp = value;
        usingDefault = other->usingDefault;
        usingLog = other->usingLog;
    }

    inline float logscale(const float v) const
    {
        const float b = std::log(maximum / minimum) / (maximum - minimum);
        const float a = maximum / std::exp(maximum * b);
        return a * std::exp(b * v);
    }

    inline float invlogscale(const float v) const
    {
        const float b = std::log(maximum / minimum) / (maximum - minimum);
        const float a = maximum / std::exp(maximum * b);
        return std::log(v / a) / b;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        if (ev.press)
        {
            if (!sliderArea.contains(ev.pos))
                return false;

            if ((ev.mod & kModifierShift) != 0 && usingDefault)
            {
                setValue(valueDef, true);
                valueTmp = value;
                return true;
            }

            float vper;
            const double x = ev.pos.getX();
            const double y = ev.pos.getY();

            if (startPos.getY() == endPos.getY())
            {
                // horizontal
                vper = float(x - sliderArea.getX()) / float(sliderArea.getWidth());
            }
            else
            {
                // vertical
                vper = float(y - sliderArea.getY()) / float(sliderArea.getHeight());
            }

            float value;

            if (inverted)
                value = maximum - vper * (maximum - minimum);
            else
                value = minimum + vper * (maximum - minimum);

            if (value < minimum)
            {
                valueTmp = value = minimum;
            }
            else if (value > maximum)
            {
                valueTmp = value = maximum;
            }
            else if (d_isNotZero(step))
            {
                valueTmp = value;
                const float rest = std::fmod(value, step);
                value = value - rest + (rest > step / 2.0f ? step : 0.0f);
            }

            dragging = true;
            startedX = x;
            startedY = y;

            if (callback != nullptr)
                callback->sliderDragStarted(widget);

            setValue(value, true);

            return true;
        }
        else if (dragging)
        {
            if (callback != nullptr)
                callback->sliderDragFinished(widget);

            dragging = false;
            return true;
        }
        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        if (!dragging)
            return false;

        const bool horizontal = startPos.getY() == endPos.getY();
        const double x = ev.pos.getX();
        const double y = ev.pos.getY();

        if ((horizontal && sliderArea.containsX(x)) || (sliderArea.containsY(y) && !horizontal))
        {
            float vper;

            if (horizontal)
            {
                // horizontal
                vper = float(x - sliderArea.getX()) / float(sliderArea.getWidth());
            }
            else
            {
                // vertical
                vper = float(y - sliderArea.getY()) / float(sliderArea.getHeight());
            }

            float value;

            if (inverted)
                value = maximum - vper * (maximum - minimum);
            else
                value = minimum + vper * (maximum - minimum);

            if (value < minimum)
            {
                valueTmp = value = minimum;
            }
            else if (value > maximum)
            {
                valueTmp = value = maximum;
            }
            else if (d_isNotZero(step))
            {
                valueTmp = value;
                const float rest = std::fmod(value, step);
                value = value - rest + (rest > step / 2.0f ? step : 0.0f);
            }

            setValue(value, true);
        }
        else if (horizontal)
        {
            if (x < sliderArea.getX())
                setValue(inverted ? maximum : minimum, true);
            else
                setValue(inverted ? minimum : maximum, true);
        }
        else
        {
            if (y < sliderArea.getY())
                setValue(inverted ? maximum : minimum, true);
            else
                setValue(inverted ? minimum : maximum, true);
        }

        return true;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        printf("scroll event\n");
        return false;
    }

    float getNormalizedValue() const noexcept
    {
        const float diff = maximum - minimum;
        return ((usingLog ? invlogscale(value) : value) - minimum) / diff;
    }

    void setRange(const float min, const float max) noexcept
    {
        DISTRHO_SAFE_ASSERT_RETURN(max > min, );

        if (value < min)
        {
            valueTmp = value = min;
            widget->repaint();
        }
        else if (value > max)
        {
            valueTmp = value = max;
            widget->repaint();
        }

        minimum = min;
        maximum = max;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        if (d_isEqual(value, value2))
            return false;

        valueTmp = value = value2;
        widget->repaint();

        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->sliderValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("SliderEventHandler::setValue");
        }

        return true;
    }

    void setInverted(bool inv) noexcept
    {
        if (inverted == inv)
            return;

        inverted = inv;
        widget->repaint();
    }
};

// --------------------------------------------------------------------------------------------------------------------

SliderEventHandler::SliderEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

SliderEventHandler::SliderEventHandler(SubWidget *const self, const SliderEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

SliderEventHandler &SliderEventHandler::operator=(const SliderEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

SliderEventHandler::~SliderEventHandler()
{
    delete pData;
}

float SliderEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool SliderEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

float SliderEventHandler::getNormalizedValue() const noexcept
{
    return pData->getNormalizedValue();
}

void SliderEventHandler::setDefault(const float def) noexcept
{
    pData->valueDef = def;
    pData->usingDefault = true;
}

void SliderEventHandler::setSliderArea(const double x, const double y,
                                       const double w, const double h) noexcept
{
    pData->sliderArea = Rectangle<double>(x, y, w, h);
}

void SliderEventHandler::setRange(const float min, const float max) noexcept
{
    pData->setRange(min, max);
}

void SliderEventHandler::setStep(const float step) noexcept
{
    pData->step = step;
}

void SliderEventHandler::setUsingLogScale(const bool yesNo) noexcept
{
    pData->usingLog = yesNo;
}

void SliderEventHandler::setStartPos(const int x, const int y) noexcept
{
    pData->startPos = Point<int>(x, y);
}

void SliderEventHandler::setEndPos(const int x, const int y) noexcept
{
    pData->endPos = Point<int>(x, y);
}

void SliderEventHandler::setInverted(const bool inv) noexcept
{
    pData->setInverted(inv);
}

bool SliderEventHandler::isInverted() noexcept
{
    return pData->inverted;
}

void SliderEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool SliderEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool SliderEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool SliderEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
}
// end slider
// --------------------------------------------------------------------------------------------------------------------

// begin spinner

struct SpinnerEventHandler::PrivateData
{
    SpinnerEventHandler *const self;
    SubWidget *const widget;
    SpinnerEventHandler::Callback *callback;

    float minimum;
    float maximum;
    float step;
    float value;
    Rectangle<double> incArea;
    Rectangle<double> decArea;

    PrivateData(SpinnerEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          step(0.0f),
          value(0.5f),
          incArea(),
          decArea()
    {
    }

    PrivateData(SpinnerEventHandler *const s, SubWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          step(other->step),
          value(other->value),
          incArea(other->incArea),
          decArea(other->decArea)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        step = other->step;
        value = other->value;
        incArea = other->incArea;
        decArea = other->decArea;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1)
            return false;

        if (ev.press)
        {
            if (!incArea.contains(ev.pos) && !decArea.contains(ev.pos))
                return false;

            if (incArea.contains(ev.pos))
                value += step;

            if (decArea.contains(ev.pos))
                value -= step;

            value = clamp(value, maximum, minimum);

            setValue(value, true);

            return true;
        }

        return false;
    }

    bool motionEvent(const Widget::MotionEvent &ev)
    {
        return false;
    }

    bool scrollEvent(const Widget::ScrollEvent &ev)
    {
        if (!widget->contains(ev.pos))
            return false;

        auto dir = ev.direction;
        switch (dir)
        {
        case ScrollDirection::kScrollUp:
            value += step;
            break;
        case ScrollDirection::kScrollDown:
            value -= step;
        default:
            break;
        }
        setValue(value, true);
        return true;
    }

    void setRange(const float min, const float max) noexcept
    {
        DISTRHO_SAFE_ASSERT_RETURN(max > min, );

        minimum = min;
        maximum = max;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        value = clamp(value2, maximum, minimum);
        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->spinnerValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("SpinnerEventHandler::setValue");
        }

        return true;
    }
};

// --------------------------------------------------------------------------------------------------------------------

SpinnerEventHandler::SpinnerEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

SpinnerEventHandler::SpinnerEventHandler(SubWidget *const self, const SpinnerEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

SpinnerEventHandler &SpinnerEventHandler::operator=(const SpinnerEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

SpinnerEventHandler::~SpinnerEventHandler()
{
    delete pData;
}

float SpinnerEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool SpinnerEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

void SpinnerEventHandler::setRange(const float min, const float max) noexcept
{
    pData->setRange(min, max);
}

void SpinnerEventHandler::setStep(const float step) noexcept
{
    pData->step = step;
}

void SpinnerEventHandler::setIncrementArea(const double x, const double y, const double w, const double h) noexcept
{
    pData->incArea = Rectangle<double>(x, y, w, h);
}

void SpinnerEventHandler::setDecrementArea(const double x, const double y, const double w, const double h) noexcept
{
    pData->decArea = Rectangle<double>(x, y, w, h);
}

void SpinnerEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

Rectangle<double> SpinnerEventHandler::getIncrementArea() noexcept
{
    return pData->incArea;
}

Rectangle<double> SpinnerEventHandler::getDecrementArea() noexcept
{
    return pData->decArea;
}

bool SpinnerEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}

bool SpinnerEventHandler::motionEvent(const Widget::MotionEvent &ev)
{
    return pData->motionEvent(ev);
}

bool SpinnerEventHandler::scrollEvent(const Widget::ScrollEvent &ev)
{
    return pData->scrollEvent(ev);
    printf("scrollEvent \n");
}
// end spinner

// begin radio

struct RadioEventHandler::PrivateData
{
    RadioEventHandler *const self;
    SubWidget *const widget;
    RadioEventHandler::Callback *callback;

    // struct Option
    // {
    //     const char *name;
    //     float value;
    //     Rectangle<double> hitbox;

    //     Option(const char *nm, float val)
    //     {
    //         name = nm;
    //         value = val;
    //     }
    // };

    float minimum;
    float maximum;
    float value;
    std::vector<Option> options;

    PrivateData(RadioEventHandler *const s, SubWidget *const w)
        : self(s),
          widget(w),
          callback(nullptr),
          minimum(0.0f),
          maximum(1.0f),
          value(0.0f)
    {
    }

    PrivateData(RadioEventHandler *const s, SubWidget *const w, PrivateData *const other)
        : self(s),
          widget(w),
          callback(other->callback),
          minimum(other->minimum),
          maximum(other->maximum),
          value(other->value)
    {
    }

    void assignFrom(PrivateData *const other)
    {
        callback = other->callback;
        minimum = other->minimum;
        maximum = other->maximum;
        value = other->value;
    }

    bool mouseEvent(const Widget::MouseEvent &ev)
    {
        if (ev.button != 1 || !ev.press || !widget->contains(ev.pos))
            return false;

        else
        {
            for (auto &hb : options)
            {
                if (hb.hitbox.contains(ev.pos))
                {
                    setValue(hb.value, true);
                    return true;
                }
            }
        }

        return false;
    }

    bool setValue(const float value2, const bool sendCallback)
    {
        value = clamp(value2, maximum, minimum);
        if (sendCallback && callback != nullptr)
        {
            try
            {
                callback->radioValueChanged(widget, value);
            }
            DISTRHO_SAFE_EXCEPTION("RadioEventHandler::setValue");
        }

        return true;
    }

    void addOption(const char *name, float value)
    {
        options.emplace_back(Option(name, value));
        initHitboxes();
    }

    void getOptions(std::vector<Option> &returnOptions)
    {
        returnOptions = options;
    }

    void getHitboxes(std::vector<Rectangle<double>> &hitboxes)
    {
        for (auto option : options)
        {
            hitboxes.push_back(option.hitbox);
        }
    }

    void initHitboxes()
    {
        const int numOptions = options.size();
        const double widgetHeight = widget->getHeight();
        const double widgetWidth = widget->getWidth();
        const double hitboxHeight = widgetHeight / static_cast<float>(numOptions);
        const double hitboxWidth = widgetWidth;
        double y = 0;
        for (auto &hb : options)
        {
            hb.hitbox = Rectangle<double>(0, y, hitboxWidth, hitboxHeight);
            y += hitboxHeight;
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------

RadioEventHandler::RadioEventHandler(SubWidget *const self)
    : pData(new PrivateData(this, self)) {}

RadioEventHandler::RadioEventHandler(SubWidget *const self, const RadioEventHandler &other)
    : pData(new PrivateData(this, self, other.pData)) {}

RadioEventHandler &RadioEventHandler::operator=(const RadioEventHandler &other)
{
    pData->assignFrom(other.pData);
    return *this;
}

RadioEventHandler::~RadioEventHandler()
{
    delete pData;
}

float RadioEventHandler::getValue() const noexcept
{
    return pData->value;
}

bool RadioEventHandler::setValue(const float value, const bool sendCallback) noexcept
{
    return pData->setValue(value, sendCallback);
}

void RadioEventHandler::addOption(const char *optName, float val)
{
    pData->addOption(optName, val);
}

void RadioEventHandler::getHitboxes(std::vector<Rectangle<double>> &hitboxes)
{
    pData->getHitboxes(hitboxes);
}

void RadioEventHandler::getOptions(std::vector<Option> &options)
{
    pData->getOptions(options);
}

void RadioEventHandler::initHitboxes()
{
    pData->initHitboxes();
}

void RadioEventHandler::setCallback(Callback *const callback) noexcept
{
    pData->callback = callback;
}

bool RadioEventHandler::mouseEvent(const Widget::MouseEvent &ev)
{
    return pData->mouseEvent(ev);
}
// end Radio

END_NAMESPACE_DGL
