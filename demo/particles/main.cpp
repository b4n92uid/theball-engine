#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

inline void DisplayOnly(ParticlesEmiter::Array& array, int& index)
{
    for(unsigned i = 0; i < array.size(); i++)
        array[i]->SetEnable(false);

    index = max(0, min((int)array.size() - 1, index));

    array[index]->SetEnable(true);
}

void mainapp()
{
    SDLDevice* ge = new SDLDevice;

    ge->Window("Particles", Vector2i(800, 600), 32, false);
    ge->SetGrabInput(true);
    ge->SetMouseVisible(false);

    SceneManager* sceneManager = ge->GetSceneManager();

    OrbitalCamera* maincam = new OrbitalCamera;
    sceneManager->AddCamera("maincam", maincam);

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneManager->AddParallelScene("1", meshScene);

    Plane* ground = new Plane(2, 1);
    ground->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", ground);

    ParticlesParallelScene* mainscene = new ParticlesParallelScene;
    sceneManager->AddParallelScene("2", mainscene);

    ParticlesEmiter::Array array;
    int diplay = 0;

    ParticlesEmiter* fire = new ParticlesEmiter;
    fire->SetPos(Vector3f(0, 0, 0));
    fire->SetGravity(Vector3f(0, 0.005, 0));
    fire->SetFreeMove(0.05);
    fire->SetTexture("../../medias/particles/fire.bmp");
    fire->SetLifeInit(2);
    fire->SetLifeDown(0.05);
    fire->SetNumber(1000);
    fire->SetContinousMode(true);
    fire->Build();
    mainscene->AddParticlesEmiter("", fire);
    array.push_back(fire);

    ParticlesEmiter* light = new ParticlesEmiter;
    light->SetPos(Vector3f(-2, 0, -2));
    light->SetEndPos(Vector3f(4, 0, 4));
    light->SetGravity(Vector3f(0, 0.005, 0));
    light->SetFreeMove(0);
    light->SetTexture("../../medias/particles/light.bmp");
    light->SetLifeInit(2);
    light->SetLifeDown(0.05);
    light->SetNumber(1000);
    light->SetContinousMode(true);
    light->Build();
    mainscene->AddParticlesEmiter("", light);
    array.push_back(light);

    DisplayOnly(array, diplay);

    FpsManager* fpsMng = ge->GetFpsManager();
    fpsMng->SetRunFps(60);

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        if(fpsMng->DoARender())
        {
            ge->PollEvent();

            maincam->OnEvent(event);

            if(event->notify == EventManager::EVENT_KEY_DOWN)
            {
                if(event->keyState[EventManager::KEY_PAGEUP])
                    DisplayOnly(array, ++diplay);

                if(event->keyState[EventManager::KEY_PAGEDOWN])
                    DisplayOnly(array, --diplay);

                if(event->keyState[EventManager::KEY_ESCAPE])
                    done = true;
            }

            ge->BeginScene();
            sceneManager->Render();
            ge->EndScene();
        }

        fpsMng->Update();
    }

    delete ge;
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
