/*
 * File:   VectorBox.cpp
 * Author: b4n92uid
 *
 * Created on 18 juin 2010, 15:46
 */

#include "VectorBox.h"
#include "Skin.h"

using namespace std;
using namespace tbe;
using namespace tbe::gui;

VectorBox::VectorBox()
{
    m_lay.setOrientation(Layout::Horizental);
    m_lay.addControl(&m_xValue);
    m_lay.addControl(&m_yValue);
    m_lay.addControl(&m_zValue);

    m_xValue.setStep(0.1);
    m_yValue.setStep(0.1);
    m_zValue.setStep(0.1);
}

VectorBox::~VectorBox()
{
}

bool VectorBox::onEvent(const EventManager& event)
{
    m_activate = m_xValue.onEvent(event) || m_yValue.onEvent(event) || m_zValue.onEvent(event);
    return m_activate;
}

void VectorBox::setSkin(const GuiSkin& gui)
{
    setSize(gui.vectorBoxSize);
    m_lay.setSize(gui.vectorBoxSize);

    Vector2f switchSize = gui.vectorBoxSize;
    switchSize.x /= 3;

    m_xValue.setBackground(gui.vectorBox);
    m_xValue.setSize(switchSize);
    m_xValue.setPencil(gui.pencil);

    m_yValue.setBackground(gui.vectorBox);
    m_yValue.setSize(switchSize);
    m_yValue.setPencil(gui.pencil);

    m_zValue.setBackground(gui.vectorBox);
    m_zValue.setSize(switchSize);
    m_zValue.setPencil(gui.pencil);
}

void VectorBox::objectRender()
{
    m_lay.setPos(m_pos);
    m_lay.setSize(m_size);
    m_lay.update();

    m_xValue.render();
    m_yValue.render();
    m_zValue.render();
}

void VectorBox::setRange(float min, float max)
{
    m_xValue.setRange(new SwitchNumeric<float>::Range(min, max));
    m_yValue.setRange(new SwitchNumeric<float>::Range(min, max));
    m_zValue.setRange(new SwitchNumeric<float>::Range(min, max));
}

void VectorBox::setValue(Vector3f value)
{
    m_xValue.setValue(value.x);
    m_yValue.setValue(value.y);
    m_zValue.setValue(value.z);
}

Vector3f VectorBox::getValue()
{
    return Vector3f(
                    m_xValue.getValue(),
                    m_yValue.getValue(),
                    m_zValue.getValue()
                    );
}
