/* 
 * File:   PostProcessManager.h
 * Author: b4n92uid
 *
 * Created on 4 janvier 2010, 14:39
 */

#ifndef _POSTPROCESSMANAGER_H
#define	_POSTPROCESSMANAGER_H

#include <string>
#include <map>

#include "Mathematics.h"
#include "Shader.h"
#include "Rtt.h"

namespace tbe
{

namespace scene
{
class SceneManager;
}

namespace ppe
{

/**
 * \brief Affichage de couche pour la superposition des effets apres-traitement
 */
class Layer
{
public:
    Layer();
    ~Layer();

    void Begin();
    void Draw(bool autoSetup = true);
    void End();

protected:
    GLuint m_renderId;
};

/**
 * \brief Class de base pour tout les effets apres-traitement
 */
class Effect
{
public:
    Effect(Vector2i frameSize = 128);
    virtual ~Effect();

    virtual void Process(Rtt* rtt) = 0;

    void SetEnable(bool enable);
    bool IsEnable() const;

    void SetRttFrameSize(Vector2i size);
    Vector2i GetRttFrameSize();

    typedef std::map<std::string, Effect*> Map;
    typedef std::vector<Effect*> Array;

protected:
    Shader m_processShader;
    Layer m_layer;
    Rtt* m_workRtt;
    bool m_enable;

};

/**
 * \brief Gestionnaire d'effets apres-traitement
 */
class PostProcessManager
{
public:
    PostProcessManager();
    PostProcessManager(const PostProcessManager& orig);
    virtual ~PostProcessManager();

    void Setup(Vector2i screenSize);
    void Render();
    void ClearAll();

    void AddPostEffect(std::string name, Effect* effect);
    Effect* GetPostEffect(std::string name);

    static void BeginPostProcess();
    static void EndPostProcess();

    Rtt* GetRtt() const;

private:
    Effect::Map m_postEffects;
    Vector2i m_screenSize;
    Rtt* m_rttRender;
    Layer m_layer;
};

}
}

#endif	/* _POSTPROCESSMANAGER_H */

