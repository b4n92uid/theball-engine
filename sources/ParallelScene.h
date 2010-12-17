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
#include <map>

#include "Node.h"
#include "Iterator.h"

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

    void Clear()
    {
        for(unsigned i = 0; i < m_nodes.size(); i++)
            if(!m_nodes[i]->HasParent())
                if(!m_nodes[i]->IsLockPtr())
                    delete m_nodes[i], m_nodes[i] = NULL;

        m_nodes.clear();
    }

    void Register(T* node)
    {
        if(std::find(m_nodes.begin(), m_nodes.end(), node) != m_nodes.end())
            throw Exception("NodeRegister::Register; child already exist");

        node->SetParallelScene(this);

        m_nodes.push_back(node);
    }

    void UnRegister(T* node, bool deleteptr)
    {
        typename std::vector<T*>::iterator it = std::find(m_nodes.begin(), m_nodes.end(), node);

        if(it == m_nodes.end())
            throw Exception("NodeRegister::UnRegister; cannot found child");

        if(deleteptr)
            delete (*it);

        m_nodes.erase(it);
    }

    Iterator<T*> GetIterator()
    {
        return Iterator<T*>(m_nodes);
    }

    std::vector<T*> m_nodes;
};

}
}

#endif	/* _PARALLELSCENE_H */

