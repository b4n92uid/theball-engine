#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("Bld Parser", Vector2i(800, 600), 32, false, 4);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneMng = engine->GetSceneManager();

    MeshParallelScene* nodeScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", nodeScene);

    OrbitalCamera* camera = new OrbitalCamera;
    sceneMng->AddCamera("cam", camera);



    EventManager* event = engine->GetEventManager();

    bool done = false;

    while(!done)
    {
        engine->PollEvent();

        camera->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;
        }

        engine->BeginScene();
        sceneMng->Render();
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
        cout << "Fatal Error: " << endl << e.what() << endl;
    }

    return 0;
}
