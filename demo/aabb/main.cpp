#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

void mainapp()
{
    SDLDevice* ge = new SDLDevice;
    ge->Window("AABB", Vector2i(800, 600), 32, false);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneMng = ge->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", meshScene);

    OrbitalCamera* cam = new OrbitalCamera;
    sceneMng->AddCamera("cam", cam);

    OBJMesh* palm = new OBJMesh("../../medias/obj/palm.obj");
    meshScene->AddMesh("", palm);

    const AABB& aabb = palm->GetAabb();

    Box* box = new Box((aabb.max - aabb.min) / 2.0f);
    box->SetPos((aabb.min + aabb.max) / 2.0f);
    box->GetMaterial("main")->SetFaceType(Material::LINE_STRIP);
    meshScene->AddMesh("", box);

    Axes* amin = new Axes(2, 2);
    amin->SetPos(aabb.min);
    meshScene->AddMesh("", amin);
    Axes* amax = new Axes(2, 2);
    amax->SetPos(aabb.max);
    meshScene->AddMesh("", amax);

    DiriLight* light = new DiriLight;
    sceneMng->AddDynamicLight("light", light);

    sceneMng->SetAmbientLight(0.25);

    GuiManager* guiMng = ge->GetGuiManager();

    TextBox* infoDisplay = guiMng->AddTextBox("");
    infoDisplay->SetStretch(true);
    infoDisplay->SetPos(10);
    infoDisplay->SetPencil(Pencil("../../medias/fonts/comicbd.ttf", 12));

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        ge->PollEvent();

        cam->OnEvent(event);

        if(event->keyState[EventManager::KEY_ESCAPE])
            done = true;

        infoDisplay->Write(Text("AABB Min: %f, %f, %f\nAABB Max: %f, %f, %f",
                                aabb.min.x, aabb.min.y, aabb.min.z,
                                aabb.max.x, aabb.max.y, aabb.max.z));

        ge->BeginScene();
        sceneMng->Render();
        guiMng->Render();
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
