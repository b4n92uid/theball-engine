#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;

void mainapp()
{
    SDLDevice* ge = new SDLDevice;
    ge->Window("Heightmap", Vector2i(800, 600), 32, false);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneMng = ge->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", meshScene);

    OrbitalCamera* cam = new OrbitalCamera;
    cam->SetDistance(64);
    sceneMng->AddCamera("cam", cam);

    Heightmap* heightmap = new Heightmap("../../medias/heightmap.raw");
    heightmap->SetMatrix(Vector3f(-128, 0, -128));
    heightmap->GetMaterial("main")->SetTexture(Texture("../../medias/obj/veg004.jpg", true));
    heightmap->GetMaterial("main")->Enable(Material::LIGHT | Material::TEXTURE);
    meshScene->AddMesh("heightmap", heightmap);

    DiriLight * light = new DiriLight;
    sceneMng->AddDynamicLight("light", light);

    sceneMng->SetAmbientLight(0.5);

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        ge->PollEvent();

        cam->OnEvent(event);

        if(event->keyState[EventManager::KEY_ESCAPE])
            done = true;

        ge->BeginScene();
        sceneMng->Render();
        ge->EndScene();
    }

    delete ge;
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
