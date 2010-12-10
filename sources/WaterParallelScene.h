/* 
 * File:   WaterParallelScene.h
 * Author: b4n92uid
 *
 * Created on 21 mai 2010, 22:31
 */

#ifndef _WATERPARALLELSCENE_H
#define	_WATERPARALLELSCENE_H

#include "ParallelScene.h"
#include "Water.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Scene parallel pour la gestion des plans d'eau
 */
class WaterParallelScene : public ParallelScene
{
public:
    WaterParallelScene();
    ~WaterParallelScene();

    void PreRender();
    
    void Render();

    void Clear();

    void RegisterWater(Water* water);
    void UnRegisterWater(Water* water, bool deleteptr = false);

    Iterator<Water*> GetIterator();

private:
    bool m_inPreRender;
    Water::Array m_nodes;
};

}
}

#endif	/* _WATERPARALLELSCENE_H */
