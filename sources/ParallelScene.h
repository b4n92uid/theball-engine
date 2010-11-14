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
    void Clear();

    bool IsChild(Node* child);

    void AddChild(Node* child);

    Node* GetChild(unsigned index);

    void DeleteChild(Node* child);

    void ReleaseChild(Node* child);
    Node* ReleaseChild(unsigned index);

    void SetSceneManager(SceneManager* sceneManager);
    SceneManager* GetSceneManager() const;

    void SetEnable(bool enable);
    bool IsEnable() const;

    typedef std::map<std::string, ParallelScene*> Map;
    typedef std::vector<ParallelScene*> Array;

protected:
    SceneManager* m_sceneManager;
    Node::Array m_nodes;
    bool m_enable;
};

}
}

#endif	/* _PARALLELSCENE_H */

