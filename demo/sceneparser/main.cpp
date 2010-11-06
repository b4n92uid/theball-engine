#include <Tbe.h>
#include <SDLDevice/SDLDevice.h>

using namespace std;

using namespace tbe;
using namespace tbe::scene;
using namespace tbe::gui;

class Jumper : public Mesh
{
};

class Teleporter : public Mesh
{
};

class MyClassFactory : public ClassFactory
{
public:

    MyClassFactory()
    {
        m_binder["Jumper"] = CreateJumper;
        m_binder["Teleporter"] = CreateTeleporter;
    }

    static Mesh* CreateJumper()
    {
        return new Jumper;
    }

    static Mesh* CreateTeleporter()
    {
        return new Teleporter;
    }

    Mesh* Instance(std::string type)
    {
        if(m_binder.count(type))
            return m_binder[type]();
        else
            return new Mesh;
    }

private:
    typedef Mesh* (*Inst)();
    std::map<std::string, Inst> m_binder;
};

void mainapp()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("Bld Parser", Vector2i(800, 600), 32, false, 4);
    engine->SetMouseVisible(false);
    engine->SetGrabInput(true);

    SceneManager* sceneMng = engine->GetSceneManager();

    MeshParallelScene* meshScene = new MeshParallelScene;
    sceneMng->AddParallelScene("nodeScene", meshScene);

    OrbitalCamera* camera = new OrbitalCamera;
    sceneMng->AddCamera("cam", camera);

    SceneParser* sceneParser = new SceneParser(sceneMng);
    sceneParser->SetClassFactory(new MyClassFactory);
    sceneParser->LoadScene("../../medias/scene.bld");

    EventManager* event = engine->GetEventManager();

    bool done = false;

    while(!done)
    {
        engine->PollEvent();

        camera->OnEvent(event);

        if(event->notify == EventManager::EVENT_KEY_DOWN)
        {
            if(event->keyState[EventManager::KEY_ESCAPE])
                done = true;
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
