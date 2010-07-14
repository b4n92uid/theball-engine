/* 
 * File:   main.h
 * Author: b4n92uid
 *
 * Created on 20 juin 2010, 13:48
 */

#ifndef MAIN_H
#define	MAIN_H

#include <Tbe.h>
#include "SDLDevice/SDLDevice.h"

void BuilGui(tbe::gui::GuiManager* guiManager)
{
    using namespace tbe;
    using namespace tbe::gui;

    Texture background("../../medias/gui/background.psd");

    Text loremIpsum;
    loremIpsum.LoadFromFile("../../medias/lorem.txt");

    // SESSION 0 ---------------------------------------------------------------
    guiManager->SetSession(0);

    guiManager->AddImage("", background)
            ->SetSize(guiManager->GetScreenSize());

    guiManager->AddLayout(Layout::Horizental, 0, 10);

    // -------- Collone 1
    guiManager->AddLayout(Layout::Vertical, 10);

    guiManager->AddButton("", "Button");

    guiManager->AddLayoutStretchSpace();

    guiManager->AddKeyConfig("");

    guiManager->AddLayoutStretchSpace();

    guiManager->AddEditBox("", "Text editable");

    guiManager->AddLayoutStretchSpace();

    Gauge* gauge = guiManager->AddGauge("", "Bar de progression");
    gauge->SetSmooth(true, 1);
    gauge->SetValue(50);

    guiManager->EndLayout();
    // --------

    guiManager->AddLayoutStretchSpace();

    // -------- Collone 2
    guiManager->AddLayout(Layout::Vertical, 10);

    SwitchString* ss = guiManager->AddSwitchString("");
    ss->Push("Choix #1");
    ss->Push("Choix #2");
    ss->Push("Choix #3");

    guiManager->AddLayoutStretchSpace();

    SwitchNumeric<int>* si = guiManager->AddSwitchNumeric<int>("");
    si->SetStep(2);
    si->SetRange(new SwitchNumeric<int>::Range(4, 10));

    guiManager->AddLayoutStretchSpace();

    SwitchNumeric<float>* sf = guiManager->AddSwitchNumeric<float>("");
    sf->SetStep(0.1);
    sf->SetRange(new SwitchNumeric<float>::Range(0, 1));

    guiManager->AddLayoutStretchSpace();

    guiManager->AddVectorBox("", 0);

    guiManager->EndLayout();
    // --------

    guiManager->AddLayoutStretchSpace();

    // -------- Collone 3
    guiManager->AddLayout(Layout::Vertical, 10);

    guiManager->AddStateShow("", "../../medias/gui/stateshow.psd", 5);

    guiManager->AddLayoutStretchSpace();

    guiManager->AddStateShow("", "../../medias/gui/checkbox.psd", 2);

    guiManager->EndLayout();
    // --------

    guiManager->EndLayout();

    // SESSION 1 ---------------------------------------------------------------
    guiManager->SetSession(1);

    guiManager->AddImage("", background)
            ->SetSize(guiManager->GetScreenSize());

    guiManager->AddLayout(Layout::Vertical, 0, 10);

    guiManager->AddLayoutStretchSpace();

    // -------- Ligne 1
    guiManager->AddLayout(Layout::Horizental, 10);

    TextBox* text = guiManager->AddTextBox("");
    text->SetDefinedSize(true);
    text->SetSize(Vector2f(512, 256));
    text->SetBackground("../../medias/gui/textboxh.psd");
    text->SetEnableBackground(true);
    text->SetBackgroundPadding(8);
    text->Write(loremIpsum);
    text->SetTextAlign(TextBox::LEFT);

    TextBox* label = guiManager->AddTextBox("");
    label->SetEnableBackground(true);
    label->SetBackgroundPadding(16);
    label->SetBackground("../../medias/gui/textboxh.psd");
    label->Write("theBall Engine\nby b4n92uid");

    guiManager->EndLayout();
    // --------

    guiManager->AddLayoutStretchSpace();

    // -------- Ligne 2
    guiManager->AddLayout(Layout::Horizental, 10);

    ListBox* lb1 = guiManager->AddListBox("");
    for(unsigned i = 0; i < 16; i++)
        lb1->Push("Item index " + tools::numToStr(i));
    lb1->SetBackground("../../medias/gui/listboxv.psd");
    lb1->SetBackgroundPadding(16);
    lb1->Update();

    ListBox* lb2 = guiManager->AddListBox("");
    for(unsigned i = 0; i < 16; i++)
        lb2->Push("Item index " + tools::numToStr(i));
    lb2->SetBackground("../../medias/gui/listboxh.psd");
    lb2->SetBackgroundPadding(16);
    lb2->SetDefinedSize(true);
    lb2->SetSize(Vector2f(192, 96));
    lb2->Update();

    guiManager->EndLayout();
    // --------

    guiManager->EndLayout();

    // -------------------------------------------------------------------------
    guiManager->SetSession(0);

}

#endif	/* MAIN_H */

