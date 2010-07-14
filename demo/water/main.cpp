#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

void mainapp(SDLDevice* ge)
{
    ge->Window("Water", Vector2i(800, 600), 32, false, 4);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneManager = ge->GetSceneManager();
    sceneManager->SetZFar(1024);
    sceneManager->UpdateViewParameter();

    OrbitalCamera* cam = new OrbitalCamera;
    cam->SetDistance(48);
    sceneManager->AddCamera("cam", cam);

    Texture skyboxTexture[6] = {
        "../../medias/skybox/front.bmp",
        "../../medias/skybox/back.bmp",
        "../../medias/skybox/top.bmp",
        "../../medias/skybox/bottom.bmp",
        "../../medias/skybox/left.bmp",
        "../../medias/skybox/right.bmp",
    };

    SkyBox* sky = sceneManager->GetSkybox();
    sky->SetTextures(skyboxTexture);
    sky->SetEnable(true);

    DiriLight* light = new DiriLight;
    light->SetPos(Vector3f(-1, 1, -1));
    light->SetAmbient(0);
    light->SetDiffuse(1);
    light->SetSpecular(1);

    sceneManager->AddDynamicLight("light", light);
    sceneManager->SetAmbientLight(0.2);

    // Meshs -------------------------------------------------------------------

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneManager->AddParallelScene("nodeScene", meshScene);

    OBJMesh* landscape = new OBJMesh("../../medias/obj/landscape.obj");
    meshScene->AddMesh("", landscape);

    // Water -------------------------------------------------------------------

    WaterParallelScene* waterScene = new WaterParallelScene;
    sceneManager->AddParallelScene("waterScene", waterScene);

    Water* waterPlane = new Water;
    waterPlane->SetNormalMap(Texture("../../medias/textures/waveNM.png", true));
    waterPlane->SetSize(1024);
    waterPlane->SetUvRepeat(128);
    waterPlane->SetBlend(0.5);
    waterPlane->SetDeform(0.1);
    waterPlane->SetSpeed(0.0001);
    waterScene->AddWater("", waterPlane);

    // Gui ---------------------------------------------------------------------

    GuiManager* gui = ge->GetGuiManager();

    Image* debugReflection = gui->AddImage("", waterPlane->GetReflection().GetColor());
    debugReflection->SetPos(5);
    debugReflection->SetSize(128);
    debugReflection->SetDrawDiriction(Image::DRAW_UPPER_RIGHT);

    Image* debugRefraction = gui->AddImage("", waterPlane->GetRefraction().GetColor());
    debugRefraction->SetPos(Vector2f(128 + 10, 5));
    debugRefraction->SetSize(128);
    debugRefraction->SetDrawDiriction(Image::DRAW_UPPER_RIGHT);

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        ge->PollEvent();

        cam->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->keyState[EventManager::KEY_TAB])
            {
                debugReflection->SetEnable(!debugReflection->IsEnable());
                debugRefraction->SetEnable(!debugRefraction->IsEnable());
            }

            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;
        }

        ge->BeginScene();

        waterScene->SetEnable(false);

        waterPlane->BeginReflection();
        sceneManager->Render(false);
        waterPlane->EndReflection();

        waterPlane->BeginRefraction();
        sceneManager->Render(false);
        waterPlane->EndRefraction();

        waterScene->SetEnable(true);

        sceneManager->Render();

        gui->Render();

        ge->EndScene();
    }
}

int main(int argc, char** argv)
{
    SDLDevice* ge;

    try
    {
        mainapp(ge = new SDLDevice);
    }

    catch(std::exception& e)
    {
        delete ge, ge = NULL;

        cout << "Fatal Error: " << endl << e.what() << endl;
    }

    delete ge;
    return 0;
}
