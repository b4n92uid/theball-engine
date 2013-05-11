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
 * inclus des fonctions commune de rendue et de libération mémoire
 */
class ParallelScene
{
public:
    ParallelScene();
    virtual ~ParallelScene();

    /// Methode virtual pure pour le rendue
    virtual void render() = 0;

    /// Methode la liberation memoire
    virtual void clear() = 0;

    void setSceneManager(SceneManager* sceneManager);
    SceneManager* getSceneManager() const;

    void setEnable(bool enable);
    bool isEnable() const;

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
        clear();
    }

    unsigned count()
    {
        return m_nodes.size();
    }

    void clear()
    {
        m_nodes.clear();
    }

    bool isRegistred(T* node)
    {
        const std::type_info& ti = typeid (node);

        if(std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end())
            return true;

        return false;
    }

    void registerNode(T* node)
    {
        const std::type_info& ti = typeid (node);

        if(std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end())
            throw Exception("NodeRegister::Register<%1%>; child already exist") % ti.name();

        m_nodes.push_back(node);
    }

    void unregisterNode(T* node, bool deleteptr = false)
    {
        typename std::vector<T*>::iterator it = std::find(m_nodes.begin(), m_nodes.end(), node);

        const std::type_info& ti = typeid (node);

        if(it == m_nodes.end())
            throw Exception("NodeRegister::UnRegister<%1%>; cannot found child") % ti.name();

        if(deleteptr)
            delete (*it);

        m_nodes.erase(it);
    }

    Iterator<T*> iterator()
    {
        return Iterator<T*>(m_nodes);
    }

protected:
    std::vector<T*> m_nodes;
};

}
}

#endif	/* _PARALLELSCENE_H */

