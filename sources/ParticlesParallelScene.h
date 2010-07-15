/* 
 * File:   ParticlesParallelScene.h
 * Author: b4n92uid
 *
 * Created on 30 avril 2010, 23:09
 */

#ifndef _PARTICLESPARALLELSCENE_H
#define	_PARTICLESPARALLELSCENE_H

#include "ParallelScene.h"
#include "Particles.h"

namespace tbe
{
namespace scene
{

/**
 * \brief Scene parallele de rendue des particules
 */
class ParticlesParallelScene : public ParallelScene
{
public:
    ParticlesParallelScene();
    ~ParticlesParallelScene();

    void Render();

    void Clear();

    void AddParticlesEmiter(std::string name, ParticlesEmiter* node);

    ParticlesEmiter* GetParticlesEmiter(std::string name);

    ParticlesEmiter* ReleaseParticlesEmiter(std::string name);
    void ReleaseParticlesEmiter(ParticlesEmiter* node);

    void DeleteParticlesEmiter(std::string name);
    void DeleteParticlesEmiter(ParticlesEmiter* node);

    template <typename T> T* GetParticlesEmiter(std::string name)
    {
        return static_cast<T*>(GetParticlesEmiter(name));
    }

    template <typename T> T* ReleaseParticlesEmiter(std::string name)
    {
        return static_cast<T*>(ReleaseParticlesEmiter(name));
    }

protected:
    ParticlesEmiter::Map m_nodes;
};

}
}

#endif	/* _PARTICLESPARALLELSCENE_H */
