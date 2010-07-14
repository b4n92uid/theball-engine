#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::ppe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

#define POINT_LIGHT

inline void DisplayOnly(Effect::Array& effects, int& index)
{
    for(unsigned i = 0; i < effects.size(); i++)
        effects[i]->SetEnable(false);

    index = max(0, min(index, (int)effects.size() - 1));

    effects[index]->SetEnable(true);
}

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("Post Process Effect", Vector2i(800, 600), 32, false);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneManager = engine->GetSceneManager();

    MeshParallelScene* nodeScene = new MeshParallelScene;
    sceneManager->AddParallelScene("nodeScene", nodeScene);

    OrbitalCamera* cam = new OrbitalCamera;
    sceneManager->AddCamera("cam", cam);

    OBJMesh* model = new OBJMesh("../../medias/obj/theroom.obj");
    nodeScene->AddMesh("model", model);

    PostProcessManager* ppeManager = engine->GetPostProcessManager();
    Rtt* ppeRtt = ppeManager->GetRtt();

    #ifdef POINT_LIGHT
    PointLight* light = new PointLight;
    light->SetPos(0);
    light->SetDiffuse(1);
    light->SetSpecular(1);
    light->SetRadius(4);

    #else
    DiriLight* light = new DiriLight;
    light->SetPos(1);
    light->SetDiffuse(1);

    #endif

    sceneManager->AddDynamicLight("light", light);
    sceneManager->SetAmbientLight(0.2);

    Effect::Array effects;
    bool usePpe = true;
    int display = 1;

    BlurEffect* blur = new BlurEffect;
    blur->SetPasse(5);
    ppeManager->AddPostEffect("", blur);
    effects.push_back(blur);

    BloomEffect* bloom = new BloomEffect;
    bloom->SetBlurPass(10);
    bloom->SetIntensity(0.9);
    bloom->SetUseThreshold(true);
    bloom->SetThreshold(0.1);
    ppeManager->AddPostEffect("", bloom);
    effects.push_back(bloom);

    DofEffect* dof = new DofEffect;
    ppeManager->AddPostEffect("", dof);
    effects.push_back(dof);

    MotionBlurEffect* mblur = new MotionBlurEffect;
    mblur->SetRttFrameSize(engine->GetWinSize());
    ppeManager->AddPostEffect("", mblur);
    effects.push_back(mblur);

    ColorEffect* color = new ColorEffect;
    color->SetFusionMode(ColorEffect::BLACK_WHITE);
    color->SetColor(Vector4f(1, 0, 0, 1));
    ppeManager->AddPostEffect("", color);
    effects.push_back(color);

    DisplayOnly(effects, display);

    EventManager* event = engine->GetEventManager();

    bool done = false;
    while(!done)
    {
        engine->PollEvent();

        cam->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->keyState[EventManager::KEY_PAGEUP])
                DisplayOnly(effects, --display);

            if(event->keyState[EventManager::KEY_PAGEDOWN])
                DisplayOnly(effects, ++display);

            if(event->keyState[EventManager::KEY_SPACE])
                usePpe = !usePpe;

            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;
        }

        engine->BeginScene();

        if(usePpe)
        {
            ppeRtt->Use(true);
            ppeRtt->Clear();
            sceneManager->Render();

            //            Vector3f pos = sceneManager->ScreenToWorld(Vector2i(400, 300));
            //            cout << pos << endl;

            ppeRtt->Use(false);

            ppeManager->Render();
        }
        else
        {
            sceneManager->Render();
        }

        engine->EndScene();
    }

    delete engine;
}

int main(int argc, char** argv)
{
    try
    {
        mainapp();
    }

    catch(std::exception& e)
    {
        cout << "Fatal Error :" << endl << e.what() << endl;
    }

    return 0;
}
