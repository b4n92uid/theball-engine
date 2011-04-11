/*
 * File:   Any.h
 * Author: b4n92uid
 *
 * Created on 24 novembre 2009, 20:53
 */

#ifndef _ANY_H
#define	_ANY_H

namespace tbe
{

/// \brief Class de stockage peut contenir n'import quelle type de donné

class Any
{
public:

    Any()
    {
        m_data = NULL;
    }

    Any(const Any& value)
    {
        m_data = NULL;

        setValue(value);
    }

    template<typename T> Any(const T& value)
    {
        m_data = NULL;

        setValue(value);
    }

    ~Any()
    {
        clear();
    }

    Any & operator=(const Any& value)
    {
        setValue(value);
        return *this;
    }

    template<typename T> Any & operator=(const T& value)
    {
        setValue(value);
        return *this;
    }

    void setValue(const Any& value)
    {
        if(m_data)
            delete m_data, m_data = NULL;

        if(value.m_data)
            m_data = value.m_data->clone();
    }

    template<typename T> void setValue(const T& value)
    {
        if(m_data)
            delete m_data, m_data = NULL;

        m_data = new DataHolder<T > (value);
    }

    template<typename T> T getValue()
    {
        DataHolder < T >* dh = static_cast<DataHolder < T >*>(m_data);
        return dh->content;
    }

    bool isNull()
    {
        return (m_data == NULL);
    }

    void clear()
    {
        if(m_data)
            delete m_data, m_data = NULL;
    }

private:

    class Data
    {
    public:
        virtual Data* clone() = 0;
    };

    template<typename T> class DataHolder : public Data
    {
    public:

        DataHolder(const T& value = 0)
        {
            content = value;
        }

        Data* clone()
        {
            return new DataHolder<T > (content);
        }

        virtual ~DataHolder()
        {
        }

        T content;
    };

    Data* m_data;
};

}

#endif	/* _ANY_H */

