/* 
 * File:   WaterParallelScene.h
 * Author: b4n92uid
 *
 * Created on 21 mai 2010, 22:31
 */

#ifndef _WATERPARALLELSCENE_H
#define	_WATERPARALLELSCENE_H

#include "ParallelScene.h"

namespace tbe
{
namespace scene
{

class Water;

/**
 * \brief Scene parallel pour la gestion des plans d'eau
 */
class WaterParallelScene : public ParallelSceneRegister<Water>
{
public:
    WaterParallelScene();
    ~WaterParallelScene();

    void addOffscreenScene(ParallelScene*);
    
    void render();

private:
    std::vector<ParallelScene*> m_offscreenScene;
};

}
}

#endif	/* _WATERPARALLELSCENE_H */
