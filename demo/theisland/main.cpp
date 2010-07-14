#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>
#include <fmod.h>

#include "GrassLand.h"
#include "Palm.h"

#define ENABLE_PALM_RENDING
#define ENABLE_GRASS_RENDING
#define ENABLE_WATER_RENDING
#define ENABLE_PERPIXEL_LIGHTING

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;
using namespace tbe::ticks;
using namespace tbe::ppe;

void EnableShader(Mesh* m, bool b)
{
    Material::Array materials = m->GetAllMaterial();

    for(unsigned i = 0; i < materials.size(); i++)
        b ? materials[i]->Enable(Material::SHADER) : materials[i]->Disable(Material::SHADER);
}

void SetupDay(SceneManager* sceneManager)
{
    Fog* fog = sceneManager->GetFog();
    fog->SetColor(Vector4f(0.6, 0.6, 1.0, 1));

    Texture skyTex[6] = {
        "../../medias/skybox/zpos.jpg",
        "../../medias/skybox/zneg.jpg",
        "../../medias/skybox/ypos.jpg",
        "../../medias/skybox/yneg.jpg",
        "../../medias/skybox/xneg.jpg",
        "../../medias/skybox/xpos.jpg",
    };

    SkyBox* sky = sceneManager->GetSkybox();
    sky->SetTextures(skyTex);

    sceneManager->ClearLights();

    DiriLight* sun = new DiriLight;
    sun->SetPos(Vector3f(1, 1, 0.5));
    sun->SetDiffuse(0.9);
    sun->SetSpecular(0);
    sceneManager->AddDynamicLight("sun", sun);
}

void SetupMorning(SceneManager* sceneManager)
{
    Fog* fog = sceneManager->GetFog();
    fog->SetColor(Vector4f(0.5, 0.25, 0.0, 1));

    Texture skyTex[6] = {
        "../../medias/skybox/early_morning_FR.jpg",
        "../../medias/skybox/early_morning_BK.jpg",
        "../../medias/skybox/early_morning_UP.jpg",
        "../../medias/skybox/early_morning_DN.jpg",
        "../../medias/skybox/early_morning_LF.jpg",
        "../../medias/skybox/early_morning_RT.jpg",
    };

    SkyBox* sky = sceneManager->GetSkybox();
    sky->SetTextures(skyTex);

    sceneManager->ClearLights();

    DiriLight* sun = new DiriLight;
    sun->SetPos(Vector3f(0, 0.25, -1));
    sun->SetDiffuse(Vector4f(0.25, 0.10, 0.0, 1));
    sun->SetSpecular(0.5);
    sceneManager->AddDynamicLight("sun", sun);

    //    PointLight* light = new PointLight;
    //    light->SetPos(theIsland->GetMatrix() * theIsland->FindFloor(0, 0) + Vector3f(0, 8, 1));
    //    light->SetRadius(32);
    //    light->SetDiffuse(0.25);
    //    light->SetSpecular(1);
    //    sceneManager->AddDynamicLight("", light);
}

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("theIsland", Vector2i(800, 600), 32, false);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneManager = engine->GetSceneManager();

    sceneManager->SetZFar(2048);
    sceneManager->UpdateViewParameter();

    FreeFlyCamera* fcam = new FreeFlyCamera;
    fcam->SetPos(Vector3f(0, 64, 128));
    fcam->SetTarget(Vector3f(0, -1, -1));
    sceneManager->AddCamera("fcam", fcam);

    OrbitalCamera* ocam = new OrbitalCamera;
    ocam->SetCenter(Vector3f(0, 96, 0));
    ocam->SetDistance(128);
    sceneManager->AddCamera("ocam", ocam);

    UserCamera* cam = ocam;

    #ifdef ENABLE_PERPIXEL_LIGHTING
    Shader perPixelLighting;
    perPixelLighting.LoadVertexShader("../../medias/shaders/light.vert");
    perPixelLighting.LoadFragmentShader("../../medias/shaders/light.frag");
    perPixelLighting.LoadProgram();
    //    perPixelLighting.Use(true);
    //    perPixelLighting.SetUniform("lightCount", 2);
    //    perPixelLighting.Use(false);
    #endif

    // Construction de la scene ------------------------------------------------

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneManager->AddParallelScene("A", meshScene);

    OBJMesh* theIsland = new OBJMesh("../../medias/obj/theisland.obj");
    {
        // Vertex Scaling
        HardwareBuffer& hBuffer = theIsland->GetHardwareBuffer();
        Vertex* vertex = hBuffer.Lock();
        for(unsigned i = 0; i < hBuffer.GetVertexCount(); i++)
            vertex[i].pos *= 50;
        hBuffer.UnLock();

        theIsland->ComputeAabb();

        #ifdef ENABLE_PERPIXEL_LIGHTING
        Material::Array materials = theIsland->GetAllMaterial();
        for(unsigned i = 0; i < materials.size(); i++)
        {
            materials[i]->Enable(Material::BACKFACE_CULL | Material::SHADER);
            materials[i]->SetShader(perPixelLighting);
        }
        #endif
    }
    meshScene->AddMesh("", theIsland);

    #ifdef ENABLE_GRASS_RENDING
    GrassLand* grass = new GrassLand(theIsland);
    meshScene->AddMesh("", grass);
    #endif

    #ifdef ENABLE_PALM_RENDING
    PosMap palmPosMap("../../medias/obj/palmposmap.pbm");
    palmPosMap.SetPrimiter(256);

    for(unsigned i = 0; i < palmPosMap.size(); i++)
    {
        Palm* palm = new Palm;
        palm->SetPos(theIsland->GetMatrix() * theIsland->FindFloor(palmPosMap[i].x, palmPosMap[i].z));
        meshScene->AddMesh("", palm);
    }
    #endif

    // Ocean -------------------------------------------------------------------

    #ifdef ENABLE_WATER_RENDING
    WaterParallelScene* waterScene = new WaterParallelScene;
    sceneManager->AddParallelScene("B", waterScene);

    Water* waterPlane = new Water;
    waterPlane->SetNormalMap(Texture("../../medias/textures/waveNM.png", true));
    waterPlane->SetSize(1024);
    waterPlane->SetUvRepeat(128);
    waterPlane->SetBlend(0.5);
    waterPlane->SetDeform(0.1);
    waterPlane->SetSpeed(0.0001);
    waterScene->AddWater("", waterPlane);
    #endif

    // Particules --------------------------------------------------------------

    ParticlesParallelScene* particleScene = new ParticlesParallelScene;
    sceneManager->AddParallelScene("C", particleScene);

    // Eclairage ---------------------------------------------------------------

    sceneManager->SetAmbientLight(0.1);

    Fog* fog = sceneManager->GetFog();
    fog->SetStart(10);
    fog->SetEnd(2048);
    fog->SetDensity(0.1);
    fog->SetEnable(true);

    SkyBox* sky = sceneManager->GetSkybox();
    sky->SetEnable(true);

    SetupMorning(sceneManager);

    // Effet apres traitement --------------------------------------------------

    PostProcessManager* ppeManager = engine->GetPostProcessManager();
    Rtt* ppeRtt = ppeManager->GetRtt();

    if(FrameBufferObject::CheckHardware())
    {
        ppeRtt->FBO_SetUseRenderBuffer(true);
        ppeRtt->FBO_SetMultisamplesCount(4);
    }

    BloomEffect* bloom = new BloomEffect;
    bloom->SetThreshold(0.10);
    bloom->SetIntensity(0.75);
    bloom->SetBlurPass(5);
    ppeManager->AddPostEffect("", bloom);

    //    MotionBlurEffect* mblur = new MotionBlurEffect;
    //    mblur->SetIntensity(0.5);
    //    mblur->SetRttFrameSize(engine->GetWinSize());
    //    ppeManager->AddPostEffect("", mblur);

    //    DofEffect* dof = new DofEffect;
    //    ppeManager->AddPostEffect("", dof);

    // Musique de fond ---------------------------------------------------------

    FSOUND_Init(44100, 32, 0);

    EventManager* event = engine->GetEventManager();

    FpsManager* fps = engine->GetFpsManager();
    fps->SetRunFps(60);

    bool done = false;
    bool usePpe = true;

    while(!done)
    {
        if(fps->DoARender())
        {
            engine->PollEvent();

            cam->OnEvent(event);

            if(event->notify == EventManager::EVENT_KEY_DOWN)
            {
                if(event->keyState['r'])
                {
                    fcam->SetPos(8);
                    fcam->SetTarget(-1);
                    ocam->SetDistance(128);
                    ocam->SetRotate(Vector2f(180, 0));
                }

                if(event->keyState['c'])
                    sceneManager->SetCurCamera(cam = (cam == fcam)
                                               ? static_cast<UserCamera*>(ocam)
                                               : static_cast<UserCamera*>(fcam));

                if(event->keyState['p'])
                    usePpe = !usePpe;

                if(event->keyState['f'])
                    sceneManager->GetSkybox()->SetEnable(!fog->IsEnable());

                if(event->keyState[EventManager::KEY_F1])
                    SetupMorning(sceneManager);

                if(event->keyState[EventManager::KEY_F2])
                    SetupDay(sceneManager);
            }

            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;

            engine->BeginScene();

            #ifdef ENABLE_WATER_RENDING
            waterScene->SetEnable(false);
            grass->SetEnable(false);
            EnableShader(theIsland, false);

            waterPlane->BeginReflection();
            sceneManager->Render(false);
            waterPlane->EndReflection();

            waterPlane->BeginRefraction();
            sceneManager->Render(false);
            waterPlane->EndRefraction();

            grass->SetEnable(true);
            waterScene->SetEnable(true);
            EnableShader(theIsland, true);
            #endif

            if(usePpe)
            {
                ppeRtt->Use(true);
                ppeRtt->Clear();

                sceneManager->Render();

                ppeRtt->Use(false);

                ppeManager->Render();
            }

            else
            {
                sceneManager->Render();
            }

            engine->EndScene();
        }

        fps->Update();
    }

    FSOUND_Close();

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
        cout << e.what() << endl;
    }

    return 0;
}
