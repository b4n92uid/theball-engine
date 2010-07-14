#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>
#include <GL/gl.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

//#define PPE

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("Off-Screen Rending (RTT)", Vector2i(800, 600), 32, false);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneMng = engine->GetSceneManager();

    MeshParallelScene* nodeScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", nodeScene);

    OrbitalCamera* cam = new OrbitalCamera;
    sceneMng->AddCamera("cam", cam);

    OBJMesh* model = new OBJMesh("../../medias/obj/theroom.obj");
    nodeScene->AddMesh("model", model);

    PointLight* light = new PointLight;
    light->SetPos(0);
    light->SetDiffuse(1);
    light->SetRadius(4);

    sceneMng->AddDynamicLight("light", light);
    sceneMng->SetAmbientLight(0.2);

    GuiManager* guiMng = engine->GetGuiManager();

    Vector2i frameSize(400, 300);

    Rtt rtt;
    // rtt.SetMethode(Rtt::TEXTURE);
    rtt.SetFrameSize(frameSize);
    rtt.SetCaptureColor(true);
    rtt.SetCaptureDepth(true);

    #ifdef PPE
    ppe::ColorEffect effect;
    effect.SetColor(Vector4f(1, 1, 1, 0.5));
    effect.SetFusionMode(ppe::ColorEffect::AVERAGE_BLACK_WHITE);
    #endif

    Image* frame = guiMng->AddImage("", rtt.GetDepht());
    frame->SetPos(30);
    frame->SetSize(Vector2i(200, 150));
    frame->SetDrawDiriction(Image::DRAW_UPPER_RIGHT);

    EventManager* event = engine->GetEventManager();

    bool done = false;
    while(!done)
    {
        engine->PollEvent();

        cam->OnEvent(event);

        if(event->keyState[EventManager::KEY_ESCAPE])
            done = true;

        engine->BeginScene();

        rtt.Use(true);
        rtt.Clear();
        sceneMng->Render();
        rtt.Use(false);

        #ifdef PPE
        ppe::PostProcessManager::BeginPostProcess();
        effect.Process(&rtt);
        ppe::PostProcessManager::EndPostProcess();
        #endif

        sceneMng->Render();

        guiMng->Render();

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
        cout << "Fatal Error :" << endl << e.what() << endl;
    }

    return 0;
}
