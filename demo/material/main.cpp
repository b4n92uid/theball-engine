#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>
#include <bits/stl_vector.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;

inline void DisplayOnly(Mesh::Array& array, int& index)
{
    index = max(min(index, (int)array.size() - 1), 0);

    for(unsigned i = 0; i < array.size(); i++)
        array[i]->SetEnable(false);

    array[index]->SetEnable(true);
}

void mainapp()
{
    SDLDevice* ge = new SDLDevice;
    ge->Window("Material", Vector2i(800, 600), 32, false, 4);
    ge->SetMouseVisible(false);
    ge->SetGrabInput(true);

    SceneManager* sceneMng = ge->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", meshScene);

    OrbitalCamera* cam = new OrbitalCamera;
    cam->SetDistance(4);
    sceneMng->AddCamera("cam", cam);

    Mesh::Array meshArray;
    int display = 0;

    {
        meshScene->AddMesh("", new Axes(2, 2));

        // 0) color blending ---------------------------------------------------

        Box* blending = new Box(1);
        blending->GetMaterial("main")->SetTexture("../../medias/textures/stainedglass05.jpg");
        blending->GetMaterial("main")->Enable(Material::LIGHT | Material::TEXTURE
                                              | Material::BLEND_MOD | Material::COLOR | Material::VERTEX_SORT_CULL_TRICK);
        {
            HardwareBuffer& buffer = blending->GetHardwareBuffer();
            Vertex* vertexes = buffer.Lock();
            for(unsigned i = 0; i < buffer.GetVertexCount(); i++)
                vertexes[i].color.w = 0.5;
            buffer.UnLock();
        }
        meshScene->AddMesh("", blending);
        meshArray.push_back(blending);

        // 1) alpha blending ---------------------------------------------------

        Box* alpha = new Box(1);
        alpha->GetMaterial("main")->SetTexture("../../medias/textures/radiation_box.tga");
        alpha->GetMaterial("main")->Enable(Material::LIGHT | Material::TEXTURE
                                           | Material::BLEND_MOD | Material::VERTEX_SORT_CULL_TRICK);
        meshScene->AddMesh("", alpha);
        meshArray.push_back(alpha);
    }

    DisplayOnly(meshArray, display);

    #if 0
    PointLight* light = new PointLight;
    light->SetPos(4);
    light->SetRadius(4);
    light->SetAmbient(0);
    light->SetDiffuse(1);
    light->SetSpecular(1);
    #else
    DiriLight* light = new DiriLight;
    light->SetPos(1);
    light->SetAmbient(0);
    light->SetDiffuse(1);
    light->SetSpecular(1);
    #endif

    sceneMng->AddDynamicLight("light", light);
    sceneMng->SetAmbientLight(0.25);

    EventManager* event = ge->GetEventManager();

    bool done = false;
    while(!done)
    {
        ge->PollEvent();

        cam->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->notify == EventManager::EVENT_KEY_DOWN)
            {
                if(event->keyState[EventManager::KEY_ESCAPE])
                    done = true;

                if(event->keyState[EventManager::KEY_PAGEUP])
                    DisplayOnly(meshArray, ++display);

                if(event->keyState[EventManager::KEY_PAGEDOWN])
                    DisplayOnly(meshArray, --display);
            }
        }

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
