#include "main.h"

using namespace std;

using namespace tbe;
using namespace tbe::gui;

void app()
{
    SDLDevice* engine = new SDLDevice;
    engine->Window("GUI", Vector2i(800, 600), 32, false);

    GuiManager* guiManager = engine->GetGuiManager();

    GuiSkin* guiskin = new GuiSkin;

    guiskin->button("../../medias/gui/button.png");
    guiskin->buttonSize(192, 48);

    guiskin->gauge("../../medias/gui/gauge.png");
    guiskin->gaugeSize(192, 48);

    guiskin->editBox("../../medias/gui/editbox.png");
    guiskin->editBoxSize(192, 48);

    guiskin->switchBox("../../medias/gui/switch.png");
    guiskin->switchBoxSize(192, 48);

    guiskin->vectorBox("../../medias/gui/vectorbox.png");
    guiskin->vectorBoxSize(192, 48);

    guiskin->stateShowSize(48, 48);

    guiskin->pencile("../../medias/fonts/BRLNSR.TTF", 12);

    guiManager->SetSkin(guiskin);

    BuilGui(guiManager);

    EventManager* event = engine->GetEventManager();
    bool done = false;
    while(!done)
    {
        engine->PollEvent();

        if(event->notify == EventManager::EVENT_KEY_DOWN)
            switch(event->lastActiveKey.first)
            {
                case EventManager::KEY_PAGEUP:
                    guiManager->PageUp();
                    break;
                case EventManager::KEY_PAGEDOWN:
                    guiManager->PageDown();
                    break;
            }

        if(event->notify == EventManager::EVENT_WIN_QUIT)
            done = true;

        engine->BeginScene();
        guiManager->Render();
        engine->EndScene();
    }

    delete engine;
}

int main(int argc, char** argv)
{
    using namespace std;

    try
    {
        app();
    }

    catch(std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
