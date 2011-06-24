/*
 * File:   MapMarkParallelScene.h
 * Author: b4n92uid
 *
 * Created on 14 juin 2011, 23:31
 */

#ifndef MAPMARKPARALLESCENE_H
#define	MAPMARKPARALLESCENE_H

#include "ParallelScene.h"

namespace tbe
{
namespace scene
{

class MapMark;

class MapMarkParallelScene : public ParallelSceneRegister<MapMark>
{
public:
    MapMarkParallelScene();
    virtual ~MapMarkParallelScene();

    MapMark* findMark(std::string name);

    void render();
};

}
}
#endif	/* MAPMARKPARALLESCENE_H */

