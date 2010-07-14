#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("Motion Path", Vector2i(800, 600), 32, false, 4);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    // Construction de la scene ------------------------------------------------

    SceneManager* sceneMng = engine->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", meshScene);

    OrbitalCamera* cam = new OrbitalCamera;
    sceneMng->AddCamera("cam", cam);

    Box* box = new Box(0.1);
    box->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", box);

    Plane* ground = new Plane(4, 1);
    ground->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", ground);

    FpsManager* fps = engine->GetFpsManager();
    fps->SetRunFps(60);

    // Path --------------------------------------------------------------------

    Vector3f path;

    MotionPath mp;

    mp.AddKeyFrame(path);

    for(unsigned i = 0; i < 4; i++)
        mp.AddKeyFrame(path += Vector3f(0.5, 0, 0), 0.05);

    for(unsigned i = 0; i < 16; i++)
        mp.AddKeyFrame(path.Rotate(90 + (i + 1) * 180 / 16, 0), 0.1);

    for(unsigned i = 0; i < 4; i++)
        mp.AddKeyFrame(path += Vector3f(1, 0, 0), 0.05);

    for(unsigned i = 0; i < 16; i++)
        mp.AddKeyFrame(path.Rotate(270 + (16 - i - 1) * 180 / 16, 0), 0.1);

    for(unsigned i = 0; i < 4; i++)
        mp.AddKeyFrame(path += Vector3f(0.5, 0, 0), 0.05);

    // Event -------------------------------------------------------------------

    EventManager* event = engine->GetEventManager();

    bool done = false;
    while(!done)
    {
        if(fps->DoARender())
        {
            engine->PollEvent();

            cam->OnEvent(event);

            if(event->notify == EventManager::EVENT_KEY_DOWN)
            {
                if(event->keyState[EventManager::KEY_ESCAPE])
                    done = true;
            }


            mp.Update();
            box->SetMatrix(mp.GetCurrentPos());

            engine->BeginScene();
            sceneMng->Render();
            engine->EndScene();
        }

        fps->Update();
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
        cout << "Fatal Error: " << endl << e.what() << endl;
    }

    return 0;
}
