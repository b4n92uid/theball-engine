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

}
}

#endif	/* _PARALLELSCENE_H */

