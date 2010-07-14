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
    engine->Window("Primitive", Vector2i(800, 600), 32, false);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneMng = engine->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", meshScene);

    OrbitalCamera* cam = new OrbitalCamera;
    sceneMng->AddCamera("cam", cam);

    Box* box = new Box(1);
    box->SetMatrix(Vector3f(2, 0, 0));
    box->GetMaterial("main")->Enable(Material::LIGHT);
    box->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", box);

    Sphere* sphere = new Sphere(1, 40, 40);
    sphere->SetMatrix(Vector3f(-2, 0, 0));
    sphere->GetMaterial("main")->Enable(Material::LIGHT);
    sphere->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", sphere);

    Plane* plane = new Plane(4, 4);
    plane->SetMatrix(Vector3f(0, -2, 0));
    plane->GetMaterial("main")->Enable(Material::LIGHT);
    plane->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", plane);

    DiriLight* light = new DiriLight;
    sceneMng->AddDynamicLight("light", light);

    sceneMng->SetAmbientLight(0.25);

    EventManager* event = engine->GetEventManager();

    bool done = false;
    while(!done)
    {
        engine->PollEvent();

        cam->OnEvent(event);

        if(event->keyState[EventManager::KEY_ESCAPE])
            done = true;

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
