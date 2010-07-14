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
    m_lay.SetOrientation(Layout::Horizental);
    m_lay.AddControl(&m_xValue);
    m_lay.AddControl(&m_yValue);
    m_lay.AddControl(&m_zValue);

    m_xValue.SetStep(0.1);
    m_yValue.SetStep(0.1);
    m_zValue.SetStep(0.1);
}

VectorBox::~VectorBox()
{
}

bool VectorBox::OnEvent(const EventManager& event)
{
    m_activate = m_xValue.OnEvent(event) || m_yValue.OnEvent(event) || m_zValue.OnEvent(event);
    return m_activate;
}

void VectorBox::SetSkin(const GuiSkin& gui)
{
    SetSize(gui.vectorBoxSize);
    m_lay.SetSize(gui.vectorBoxSize);

    Vector2f switchSize = gui.vectorBoxSize;
    switchSize.x /= 3;

    m_xValue.SetBackground(gui.vectorBox);
    m_xValue.SetSize(switchSize);
    m_xValue.SetPencil(gui.pencile);

    m_yValue.SetBackground(gui.vectorBox);
    m_yValue.SetSize(switchSize);
    m_yValue.SetPencil(gui.pencile);

    m_zValue.SetBackground(gui.vectorBox);
    m_zValue.SetSize(switchSize);
    m_zValue.SetPencil(gui.pencile);
}

void VectorBox::ObjectRender()
{
    m_lay.SetPos(m_pos);
    m_lay.SetSize(m_size);
    m_lay.Update();

    m_xValue.Render();
    m_yValue.Render();
    m_zValue.Render();
}

void VectorBox::SetRange(float min, float max)
{
    m_xValue.SetRange(new SwitchNumeric<float>::Range(min, max));
    m_yValue.SetRange(new SwitchNumeric<float>::Range(min, max));
    m_zValue.SetRange(new SwitchNumeric<float>::Range(min, max));
}

void VectorBox::SetValue(Vector3f value)
{
    m_xValue.SetValue(value.x);
    m_yValue.SetValue(value.y);
    m_zValue.SetValue(value.z);
}

Vector3f VectorBox::GetValue()
{
    return Vector3f(
                    m_xValue.GetValue(),
                    m_yValue.GetValue(),
                    m_zValue.GetValue()
                    );
}
