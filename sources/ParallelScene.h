/* 
 * File:   ParallelScene.h
 * Author: b4n92uid
 *
 * Created on 23 novembre 2009, 15:43
 */

#ifndef _PARALLELSCENE_H
#define	_PARALLELSCENE_H

#include <string>
#include <vector>
#include <list>
#include <typeinfo>
#include <map>

#include "Node.h"
#include "Iterator.h"
#include "Tools.h"

namespace tbe
{
namespace scene
{

class SceneManager;

/**
 * \brief Class de base pour les scenes parallele
 * inclus des fonctions commune de rendue et de lib�ration m�moire
 */
class ParallelScene
{
public:
    ParallelScene();
    virtual ~ParallelScene();

    /// Methode virtual pure pour le rendue
    virtual void Render() = 0;

    /// Methode la liberation memoire
    virtual void Clear() = 0;

    void SetSceneManager(SceneManager* sceneManager);
    SceneManager* GetSceneManager() const;

    void SetEnable(bool enable);
    bool IsEnable() const;

    typedef std::vector<ParallelScene*> Array;

protected:
    SceneManager* m_sceneManager;
    bool m_enable;
};

template<typename T> class ParallelSceneRegister : public ParallelScene
{
public:

    ~ParallelSceneRegister()
    {
        Clear();
    }

    void Clear()
    {
        m_nodes.clear();
        m_rendredNodes.clear();
    }

    void Register(T* node)
    {
        const std::type_info& ti = typeid (node);

        if(std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end())
            throw Exception("NodeRegister::Register<%s>; child already exist", ti.name());

        m_nodes.push_back(node);
    }

    void UnRegister(T* node, bool deleteptr = false)
    {
        typename std::vector<T*>::iterator it = std::find(m_nodes.begin(), m_nodes.end(), node);

        const std::type_info& ti = typeid (node);

        if(it == m_nodes.end())
            throw Exception("NodeRegister::UnRegister<%s>; cannot found child", ti.name());

        if(deleteptr)
            delete (*it);

        m_nodes.erase(it);
    }

    void PushToDraw(T* node)
    {
        const std::type_info& ti = typeid (node);

        if(std::find(m_rendredNodes.begin(), m_rendredNodes.end(), node) != m_rendredNodes.end())
            throw Exception("NodeRegister::PushToDraw<%s>; child already push", ti.name());

        m_rendredNodes.push_back(node);
    }

    Iterator<T*> GetIterator()
    {
        return Iterator<T*>(m_nodes);
    }

protected:
    std::list<T*> m_rendredNodes;
    std::vector<T*> m_nodes;
};

}
}

#endif	/* _PARALLELSCENE_H */

