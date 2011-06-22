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
    bool onEvent(const EventManager& event);

    /// GuiSkin
    void setSkin(const GuiSkin& skin);

    virtual void increment() = 0;
    virtual void decrement() = 0;

protected:
    void setupRender();
    void objectRender();

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

    void setString(const std::string& str);
    std::string getString() const;

    void setString(unsigned index, const std::string& str);
    std::string getString(unsigned index) const;

    void setData(const Any& data);
    Any getData() const;

    void setData(unsigned index, const Any& data);
    Any getData(unsigned index) const;

    void setData(std::string str, const Any& data);
    Any getData(std::string str) const;

    void setCurrent(unsigned index);
    unsigned getCurrent();

    SwitchString& push(std::string str);
    SwitchString& push(std::string str, const Any& data);

    void deleteAll();
    void deleteOn(unsigned index);
    void deleteOn(std::string str);

    unsigned getCount() const;

    void increment();
    void decrement();

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

        T check(T value)
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

        setValue(0);
    }

    ~SwitchNumeric()
    {
        if(m_range)
            delete m_range;
    }

    void increment()
    {
        setValue(m_value + m_step);
    }

    void decrement()
    {
        setValue(m_value - m_step);
    }

    void setStep(T step)
    {
        this->m_step = step;
    }

    T getStep() const
    {
        return m_step;
    }

    void setValue(T value)
    {
        this->m_value = (m_range ? m_range->check(value) : value);
        math::round(this->m_value, m_step);
        m_label = tools::numToStr(m_value);
    }

    T getValue() const
    {
        return m_value;
    }

    void setRange(Range* range)
    {
        if(m_range)
            delete m_range;

        this->m_range = range;

        if(m_range)
        {
            m_value = m_range->check(m_value);
            m_label = tools::numToStr(m_value);
        }
    }

    Range* getRange() const
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
