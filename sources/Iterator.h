/*
 * File:   Iterator.h
 * Author: b4n92uid
 *
 * Created on 30 novembre 2010, 14:55
 */

#ifndef ITERATOR_H
#define	ITERATOR_H

#include <vector>

namespace tbe
{

/**
 * Class d'itiration du conteneur std::vector
 * utiliser par les noeud et les scenes parallel
 */

template<typename T> class Iterator
{
public:

    Iterator(std::vector<T>& vec)
    : m_begin(vec.begin()), m_cur(vec.begin()), m_end(vec.end())
    {
    }

    Iterator & operator++(int)
    {
        m_cur++;
        return *this;
    }

    Iterator & operator--(int)
    {
        m_cur--;
        return *this;
    }

    T & operator*()
    {
        return (*m_cur);
    }

    T & operator->()
    {
        return (*m_cur);
    }

    operator T()
    {
        return *m_cur;
    }

    operator bool()
    {
        return (m_cur < m_end && m_cur >= m_begin);
    }

private:
    typedef typename std::vector<T>::iterator vecit;
    vecit m_begin, m_cur, m_end;
};

}

#endif	/* ITERATOR_H */

