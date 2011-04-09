/* 
 * File:   LightParallelScene.h
 * Author: b4n92uid
 *
 * Created on 17 décembre 2010, 16:54
 */

#ifndef LIGHTPARALLELSCENE_H
#define	LIGHTPARALLELSCENE_H

#include "ParallelScene.h"

namespace tbe
{
namespace scene
{

class Light;

class LightParallelScene : public ParallelSceneRegister<Light>
{
public:
    LightParallelScene();
    virtual ~LightParallelScene();

    void render();
};

}
}

#endif	/* LIGHTPARALLELSCENE_H */

