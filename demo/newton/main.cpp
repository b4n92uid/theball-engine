#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>
#include <Newton/Newton.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;

void mainapp()
{
    SDLDevice* ge = new SDLDevice;
    ge->Window("Newton Game Dynamics", Vector2i(800, 600), 32, false);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneMng = ge->GetSceneManager();

    OrbitalCamera* cam = new OrbitalCamera;
    cam->SetDistance(48);
    sceneMng->AddCamera("cam", cam);

    DiriLight* light = new DiriLight;
    sceneMng->AddDynamicLight("light", light);

    sceneMng->SetAmbientLight(0.25);

    FpsManager* fpsMng = ge->GetFpsManager();
    fpsMng->SetRunFps(60);

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("", meshScene);

    NewtonParallelScene* newtonScene = new NewtonParallelScene;
    newtonScene->SetGravity(4);
    sceneMng->AddParallelScene("", newtonScene);

    OBJMesh* landscape = new OBJMesh("../../medias/obj/landscape.obj");
    meshScene->AddMesh("", landscape);

    NewtonNode* landscapeBody = new NewtonNode(newtonScene, landscape);
    landscapeBody->BuildTreeNode(landscape->GetHardwareBuffer().GetAllFace());
    newtonScene->AddNode("", landscapeBody);

    AABB primiter = landscape->GetAabb();

    for(unsigned i = 0; i < 20; i++)
    {
        Vector3f pos = tools::rand(primiter.min + Vector3f(0, 20, 0),
                                   primiter.max + Vector3f(0, 30, 0));

        Box* box = new Box(1);
        box->SetPos(pos);
        box->GetMaterial("main")->SetTexture(Texture("../../medias/textures/crate05.jpg", true));
        box->GetMaterial("main")->Enable(Material::LIGHT | Material::TEXTURE);
        meshScene->AddMesh("", box);

        NewtonNode* boxBody = new NewtonNode(newtonScene, box);
        boxBody->BuildBoxNode(box->GetSize(), 3);
        newtonScene->AddNode("", boxBody);

        pos = tools::rand(primiter.min + Vector3f(0, 20, 0),
                          primiter.max + Vector3f(0, 30, 0));

        OBJMesh* convex = new OBJMesh("../../medias/obj/convex.obj");
        convex->SetPos(pos);
        meshScene->AddMesh("", convex);

        NewtonNode* convexBody = new NewtonNode(newtonScene, convex);
        convexBody->BuildConvexNode(convex->GetHardwareBuffer().GetAllVertex(true), 3);
        newtonScene->AddNode("", convexBody);
    }

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        if(fpsMng->DoARender())
        {
            ge->PollEvent();

            cam->OnEvent(event);

            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;

            ge->BeginScene();
            sceneMng->Render();
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
