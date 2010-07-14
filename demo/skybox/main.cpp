#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;

void mainapp()
{
    SDLDevice* engine = new SDLDevice;

    engine->Window("SkyBox", Vector2i(800, 600), 32, false);
    engine->SetGrabInput(true);
    engine->SetMouseVisible(false);

    SceneManager* sceneManager = engine->GetSceneManager();

    MeshParallelScene* mainscene = new MeshParallelScene;
    sceneManager->AddParallelScene("mainscene", mainscene);

    OrbitalCamera* maincam = new OrbitalCamera;
    maincam->SetDistance(32);
    sceneManager->AddCamera("maincam", maincam);

    Texture skys[3][6] = {
        {
            "../../medias/skybox/front.bmp",
            "../../medias/skybox/back.bmp",
            "../../medias/skybox/top.bmp",
            "../../medias/skybox/bottom.bmp",
            "../../medias/skybox/left.bmp",
            "../../medias/skybox/right.bmp",
        },
        {
            "../../medias/skybox/zpos.jpg",
            "../../medias/skybox/zneg.jpg",
            "../../medias/skybox/ypos.jpg",
            "../../medias/skybox/yneg.jpg",
            "../../medias/skybox/xneg.jpg",
            "../../medias/skybox/xpos.jpg",
        },
        {
            "../../medias/skybox/early_morning_FR.jpg",
            "../../medias/skybox/early_morning_BK.jpg",
            "../../medias/skybox/early_morning_UP.jpg",
            "../../medias/skybox/early_morning_DN.jpg",
            "../../medias/skybox/early_morning_LF.jpg",
            "../../medias/skybox/early_morning_RT.jpg",
        }
    };

    SkyBox* sky = sceneManager->GetSkybox();
    sky->SetTextures(skys[0]);
    sky->SetEnable(true);

    DiriLight* sun = new DiriLight;
    sceneManager->AddDynamicLight("", sun);

    EventManager* event = engine->GetEventManager();

    bool done = false;
    while(!done)
    {
        engine->PollEvent();

        maincam->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;

            if(event->lastActiveKey.first >= '0' && event->lastActiveKey.first <= '3')
                sky->SetTextures(skys[event->lastActiveKey.first - '1']);
        }

        engine->BeginScene();
        sceneManager->Render();
        engine->EndScene();
    }

    delete engine;
}

int main(int argc, char** argv)
{
    using namespace std;

    try
    {
        mainapp();
    }

    catch(std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
