#include <Tbe.h>

#include <SDL.h>
#undef main

using namespace std;

using namespace tbe;
using namespace tbe::gui;
using namespace tbe::scene;

void app()
{
    Device* ge = new Device;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("User Device", 0);
    SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);

    ge->Setup(Vector2i(800, 600));

    SceneManager* scenemng = ge->GetSceneManager();
    scenemng->SetRatio(4 / 3);
    scenemng->SetFovy(70);
    scenemng->SetZNear(1);
    scenemng->SetZFar(512);
    scenemng->UpdateViewParameter();

    GuiManager* guimng = ge->GetGuiManager();
    TextBox* text = guimng->AddTextBox("");
    text->SetPencil(Pencil("../../medias/fonts/TrajanPro-Regular.otf", 16));
    text->SetPos(10);
    text->Write("theBall Engine User Device");

    SDL_Event event;
    bool done = false;
    while(!done)
    {
        SDL_PollEvent(&event);

        if(event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.sym == SDLK_ESCAPE)
                done = true;
        }

        else if(event.type == SDL_QUIT)
        {
            done = true;
        }

        ge->BeginScene();
        scenemng->Render();
        guimng->Render();
        ge->EndScene();

        SDL_GL_SwapBuffers();
    }

    SDL_Quit();

    delete ge;
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
