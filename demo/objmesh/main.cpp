#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;

#define PER_PIXEL_LIGHTING
#define POINT_LIGHT

inline void SwitchMaterialShader(Material::Array& materialArray, bool state)
{
    if(state)
        for(unsigned i = 0; i < materialArray.size(); i++)
            materialArray[i]->Enable(Material::SHADER);

    else
        for(unsigned i = 0; i < materialArray.size(); i++)
            materialArray[i]->Disable(Material::SHADER);
}

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("OBJ Mesh", Vector2i(800, 600), 32, false, 4);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneMng = engine->GetSceneManager();

    MeshParallelScene* nodeScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", nodeScene);

    OrbitalCamera* camera = new OrbitalCamera;
    sceneMng->AddCamera("cam", camera);

    OBJMesh* roomMesh = new OBJMesh("../../medias/obj/theroom.obj");
    nodeScene->AddMesh("roomModel", roomMesh);

    #ifdef PER_PIXEL_LIGHTING
    Material::Array materials = roomMesh->GetAllMaterial();

    Shader perpixelLight;
    perpixelLight.LoadFragmentShader("../../medias/shaders/light.frag");
    perpixelLight.LoadVertexShader("../../medias/shaders/light.vert");
    perpixelLight.LoadProgram();

    for(unsigned i = 0; i < materials.size(); i++)
        materials[i]->SetShader(perpixelLight);

    SwitchMaterialShader(materials, true);
    #endif

    #ifdef POINT_LIGHT
    PointLight* light = new PointLight;
    light->SetPos(Vector3f(-4, 4, 4));
    light->SetRadius(8);
    light->SetDiffuse(1);
    light->SetSpecular(0.75);
    #else
    DiriLight* light = new DiriLight;
    light->SetPos(1);
    light->SetDiffuse(1);
    light->SetSpecular(1);
    light->SetSpecular(0.75);
    #endif

    sceneMng->AddDynamicLight("light", light);
    sceneMng->SetAmbientLight(0.2);

    EventManager* event = engine->GetEventManager();

    bool done = false;
    bool useShader = true;

    while(!done)
    {
        engine->PollEvent();

        camera->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;

            #ifdef PER_PIXEL_LIGHTING
            if(event->keyState[EventManager::KEY_SPACE])
                SwitchMaterialShader(materials, useShader = !useShader);
            #endif
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
