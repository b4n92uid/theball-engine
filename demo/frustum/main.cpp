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
    ge->Window("Frustum Culling", Vector2i(800, 600), 32, false, 4);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneManager = ge->GetSceneManager();

    OrbitalCamera* cam = new OrbitalCamera;
    sceneManager->AddCamera("cam", cam);

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneManager->AddParallelScene("", meshScene);

    Texture texture("../../medias/textures/crate05.jpg", true);
    unsigned side = 64;

    for(unsigned i = 0; i < side; i++)
        for(unsigned j = 0; j < side; j++)
        {
            Vector3f pos = Vector3f(i - side / 2.0, 0, j - side / 2.0) * 5.0f;

            Box* box = new Box(2);
            box->SetPos(pos);
            box->GetMaterial("main")->SetTexture(texture);
            box->GetMaterial("main")->Enable(Material::TEXTURE | Material::COLOR
                                             | Material::LIGHT | Material::BACKFACE_CULL);
            meshScene->AddMesh("", box);
        }

    DiriLight* light = new DiriLight;
    sceneManager->AddDynamicLight("light", light);
    sceneManager->SetAmbientLight(0.25);

    GuiManager* guiManager = ge->GetGuiManager();

    TextBox* infoDisplay = guiManager->AddTextBox("");
    infoDisplay->SetStretch(true);
    infoDisplay->SetPos(10);
    infoDisplay->SetPencil(Pencil("../../medias/fonts/comicbd.ttf", 12));

    bool done = false;
    EventManager* event = ge->GetEventManager();
    while(!done)
    {
        ge->PollEvent();

        cam->OnEvent(event);

        if(event->keyState[EventManager::KEY_ESCAPE])
            done = true;

        if(event->notify == EventManager::EVENT_KEY_DOWN
           && event->lastActiveKey.first == EventManager::KEY_SPACE)
            meshScene->SetEnableFrustumTest(!meshScene->IsEnableFrustumTest());

        infoDisplay->Write(Text("Frustum %d\nCulled/Total nodes %d/%d",
                                meshScene->IsEnableFrustumTest(),
                                meshScene->GetFrustumCullingCount(), side * side));

        ge->BeginScene();
        sceneManager->Render();
        guiManager->Render();
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
