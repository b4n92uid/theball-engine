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
    ge->Window("Techniques", Vector2i(800, 600), 32, false, 4);
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
        //meshScene->AddMesh("", new Axes(2, 2));

        // 0) per pixel lighting -----------------------------------------------

        Shader perPixelLighting;
        perPixelLighting.LoadFragmentShader("../../medias/shaders/light.frag");
        perPixelLighting.LoadVertexShader("../../medias/shaders/light.vert");
        perPixelLighting.LoadProgram();

        OBJMesh* ppl = new OBJMesh("../../medias/obj/monkey.obj");
        {
            Material* material = ppl->GetMaterial("Material_Texture");
            material->SetShader(perPixelLighting);
            material->Enable(Material::LIGHT | Material::TEXTURE | Material::SHADER);
            material->SetSpecular(0.5);
            material->SetShininess(32);
        }
        meshScene->AddMesh("", ppl);
        meshArray.push_back(ppl);

        // 1) bump mapping -----------------------------------------------------

        Shader bumpMapLighting;
        bumpMapLighting.LoadFragmentShader("../../medias/shaders/bump.frag");
        bumpMapLighting.LoadVertexShader("../../medias/shaders/bump.vert");
        bumpMapLighting.LoadProgram();
        bumpMapLighting.Use(true);
        bumpMapLighting.SetUniform("colorMap", 0);
        bumpMapLighting.SetUniform("normalMap", 1);
        bumpMapLighting.Use(false);

        OBJMesh* bumpMap = new OBJMesh("../../medias/obj/monkey.obj");
        bumpMap->ComputeTangent();
        {
            Material* material = bumpMap->GetMaterial("Material_Texture");
            material->SetTexture(Texture("../../medias/obj/woodfloor005a_NM.jpg", true), 1);
            material->SetShader(bumpMapLighting);
            material->Enable(Material::LIGHT | Material::TEXTURE | Material::SHADER | Material::TANGENT);
            material->SetSpecular(0.5);
            material->SetShininess(32);
        }
        meshScene->AddMesh("", bumpMap);
        meshArray.push_back(bumpMap);
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
                    DisplayOnly(meshArray, --display);

                if(event->keyState[EventManager::KEY_PAGEDOWN])
                    DisplayOnly(meshArray, ++display);
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
