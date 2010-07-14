#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

// Meilleur FPS 460

void mainapp()
{
    SDLDevice* ge = new SDLDevice;
    ge->Window("FPS", Vector2i(800, 600), 32, false);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneMng = ge->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("", meshScene);

    OrbitalCamera* cam = new OrbitalCamera;
    sceneMng->AddCamera("cam", cam);

    Box* box1 = new Box(4);
    box1->GetMaterial("main")->SetTexture("../../medias/textures/crate05.jpg");
    box1->GetMaterial("main")->Enable(Material::LIGHT | Material::TEXTURE);
    meshScene->AddMesh("box1", box1);

    DiriLight* light = new DiriLight;
    sceneMng->AddDynamicLight("light", light);

    sceneMng->SetAmbientLight(0.5);

    GuiManager* guiMng = ge->GetGuiManager();

    TextBox* fpsShow = guiMng->AddTextBox("");
    fpsShow->SetStretch(true);
    fpsShow->SetPos(10);
    fpsShow->SetPencil(Pencil("../../medias/fonts/comicbd.ttf", 12));

    FpsManager* fpsMng = ge->GetFpsManager();
    fpsMng->SetRunFps(60);

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        // if(fpsMng->DoARender())
        {
            ge->PollEvent();

            cam->OnEvent(event);

            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;

            fpsShow->Write(Text("FPS Courant : %d\n"
                                "Meilleur FPS : %d",
                                fpsMng->GetCurFps(),
                                fpsMng->GetBetterFps()));

            ge->BeginScene();
            sceneMng->Render();
            guiMng->Render();
            ge->EndScene();
        }

        fpsMng->Update();
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
