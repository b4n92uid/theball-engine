#ifndef SWITCH_H
#define SWITCH_H

#include "Texture.h"
#include "Tools.h"

#include "Control.h"
#include "Pencil.h"

namespace tbe
{
namespace gui
{

/**
 * \brief Class abstraite pour la gestion des Switch
 * Controle de choix unique
 */
class Switch : public Control
{
public:
    Switch();
    Switch(Pencil font, Texture background);
    virtual ~Switch();

    /// Evenement
    bool OnEvent(const EventManager& event);

    /// GuiSkin
    void SetSkin(const GuiSkin& skin);

    virtual void Increment() = 0;
    virtual void Decrement() = 0;

protected:
    void SetupRender();
    void ObjectRender();

    enum State
    {
        DEAD,
        OVER_LEFT,
        OVER_RIGHT,
        PUSH_LEFT,
        PUSH_RIGHT,

    } m_state;
};

/**
 * \brief Peut contenire plusieur élément de type string pour un Switch
 */
class SwitchString : public Switch
{
public:
    SwitchString();

    void SetString(const std::string& str);
    std::string GetString() const;

    void SetString(unsigned index, const std::string& str);
    std::string GetString(unsigned index) const;

    void SetData(const Any& data);
    Any GetData() const;

    void SetData(unsigned index, const Any& data);
    Any GetData(unsigned index) const;

    void SetData(std::string str, const Any& data);
    Any GetData(std::string str) const;

    void SetCurrent(unsigned index);
    unsigned GetCurrent();

    SwitchString& Push(std::string str);
    SwitchString& Push(std::string str, const Any& data);

    void Delete(unsigned index);
    void Delete(std::string str);

    unsigned GetCount() const;

    void Increment();
    void Decrement();

protected:

    struct Entry
    {
        std::string str;
        Any data;
    };

    std::vector<Entry > m_content;
    std::vector<Entry >::iterator m_it;
};

/**
 * \brief Template pour l'utilisation des Switch a l'état numérique
 */
template<typename T>
class SwitchNumeric : public Switch
{
public:

    /**
     * \brief Structure pour la verification des valeurs permise
     */
    struct Range
    {

        Range(T min, T max)
        {
            this->min = min;
            this->max = max;
        }

        T Check(T value)
        {
            return std::min(std::max(value, min), max);
        }

        T min;
        T max;
    };

    SwitchNumeric()
    {
        m_range = NULL;
        m_step = T(1);

        SetValue(0);
    }

    ~SwitchNumeric()
    {
        if(m_range)
            delete m_range;
    }

    void Increment()
    {
        SetValue(m_value + m_step);
    }

    void Decrement()
    {
        SetValue(m_value - m_step);
    }

    void SetStep(T step)
    {
        this->m_step = step;
    }

    T GetStep() const
    {
        return m_step;
    }

    void SetValue(T value)
    {
        this->m_value = (m_range ? m_range->Check(value) : value);
        tools::round(this->m_value, m_step);
        m_label = tools::numToStr(m_value);
    }

    T GetValue() const
    {
        return m_value;
    }

    void SetRange(Range* range)
    {
        if(m_range)
            delete m_range;

        this->m_range = range;

        if(m_range)
        {
            m_value = m_range->Check(m_value);
            m_label = tools::numToStr(m_value);
        }
    }

    Range* GetRange() const
    {
        return m_range;
    }

protected:
    T m_value;
    T m_step;
    Range* m_range;
};

}
}

#endif // SWITCH_H
