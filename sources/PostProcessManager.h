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
    Layer(Vector2f pos = 0, Vector2f size = 1, Vector4f color = 1);
    ~Layer();

    void draw(bool autoSetup = true);

    void begin();
    void end();

    void bindTexture(unsigned layer);
    void unbindTexture(unsigned layer);

protected:
    HardwareBuffer m_renderId;
};

/**
 * \brief Class de base pour tout les effets apres-traitement
 */
class Effect
{
public:
    Effect();
    virtual ~Effect();

    virtual void process(Rtt* rtt) = 0;

    void setEnable(bool enable);
    bool isEnable() const;

    void setRttFrameSize(Vector2i size);
    Vector2i getRttFrameSize();

    typedef std::map<std::string, Effect*> Map;
    typedef std::vector<Effect*> Array;

    static Vector2i defaultSize;

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

    void setup(Vector2i viewport);
    void render();
    void clearAll();

    void setViewport(Vector2i viewport);
    Vector2i getViewport() const;

    void addPostEffect(std::string name, Effect* effect);

    Effect* getPostEffect(std::string name);

    Effect* releasePostEffect(std::string name);

    void deletePostEffect(std::string name);
    void deletePostEffect(Effect* effect);

    static void beginPostProcess(float w = 1, float h = 1);
    static void endPostProcess();

    Rtt* getRtt() const;

private:
    Effect::Map m_postEffects;
    Vector2i m_viewport;
    Rtt* m_rttRender;
    Layer m_layer;
};

}
}

#endif	/* _POSTPROCESSMANAGER_H */

