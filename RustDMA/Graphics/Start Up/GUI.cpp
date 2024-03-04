﻿#include "pch.h"
#include "GUI.h"
#include "entity.h"
#include "Form.h"
#include "Button.h"
#include "ColourPicker.h"
#include "Label.h"
#include "tab.h"
#include "TabController.h"
#include "Toggle.h"
#include "Slider.h"
#include "DropDown.h"
#include "ComboBox.h"
#include "KeyBind.h"
#include "TabListBox.h"
#include "TabListBoxController.h"
#include "TextBox.h"
#include "ConfigInstance.h"
#include "OcclusionCulling.h"
#include "ConvarAdmin.h"
int SelectedTab = 1;
int SelectedSubTab = 0;
int TabCount = 0;
int KeyBindClipBoard = 0;
EntityVector MenuEntity;
bool MenuOpen = true;
D2D1::ColorF ColourPickerClipBoard = Colour(255,255,255);
D2D1::ColorF ColourPick = Colour(0, 150, 255, 255);

void CreateGUI()
{
	MenuEntity = std::make_shared<Container>();
	auto form = std::make_shared<Form>(100, 100.0f, 400, 300, 2, 30, LIT(L"Cool Hack"), false);
	{
	auto enableplayeresp = std::make_shared<Toggle>(10, 10, LIT(L"Enable Player ESP"), &ConfigInstance.PlayerESP.Enable);
	form->Push(enableplayeresp);
	auto playercolour = std::make_shared<ColourPicker>(150, 10, &ConfigInstance.PlayerESP.Colour);
	form->Push(playercolour);
	auto playername = std::make_shared<Toggle>(10, 30, LIT(L"Player Name"), &ConfigInstance.PlayerESP.Name);
	form->Push(playername);
	auto playerdistance = std::make_shared<Toggle>(10, 50, LIT(L"Player Distance"), &ConfigInstance.PlayerESP.Distance);
	form->Push(playerdistance);
	auto playermaxdistance = std::make_shared<Slider<int>>(10, 70,150, LIT(L"Max Distance"),LIT(L"m"), 0, 1000, &ConfigInstance.PlayerESP.MaxDistance);
	form->Push(playermaxdistance);
	auto adminesp = std::make_shared<Toggle>(10, 95, LIT(L"Admin Box ESP"), &ConfigInstance.Misc.AdminESP);
	adminesp->SetValueChangedEvent([]()
		{
			std::shared_ptr<OcclusionCulling> occlusionculling = std::make_shared<OcclusionCulling>();
			if (ConfigInstance.Misc.AdminESP)
			{
				occlusionculling->WriteDebugSettings(DebugFilter::Dynamic);
				occlusionculling->WriteLayerMask(131072);
			}
			else
			{
				occlusionculling->WriteDebugSettings(DebugFilter::Off);
				occlusionculling->WriteLayerMask(0);
			}
		});
	form->Push(adminesp);
	auto watereffect = std::make_shared<Toggle>(10,115, LIT(L"Remove Water Effect"), &ConfigInstance.Misc.RemoveWaterEffect);
	watereffect->SetValueChangedEvent([]()
		{
			std::shared_ptr<ConvarAdmin> convaradmin = std::make_shared<ConvarAdmin>();
			if (ConfigInstance.Misc.RemoveWaterEffect)
				convaradmin->ClearVisionInWater(true);
		});
	form->Push(watereffect);
	auto adminflag = std::make_shared<Toggle>(10, 135, LIT(L"Admin Flag"), &ConfigInstance.Misc.AdminFlag);
	form->Push(adminflag);
	auto changetime = std::make_shared<Toggle>(10, 155, LIT(L"Change Time"), &ConfigInstance.Misc.ChangeTime);
	changetime->SetValueChangedEvent([]()
		{
			std::shared_ptr<ConvarAdmin> convaradmin = std::make_shared<ConvarAdmin>();
			if (ConfigInstance.Misc.ChangeTime)
				convaradmin->SetAdminTime(ConfigInstance.Misc.Time);
			else
				convaradmin->SetAdminTime(-1);
		});
	form->Push(changetime);

	}

	MenuEntity->Push(form);
	MenuEntity->Draw();
	MenuEntity->Update();
}

void SetFormPriority()
{
	// This sorts the host container (containerptr) which contains forms, as long as a form isn't parented to another form then this will allow it to draw over when clicked.
	// I swear to god if i need to make this work for forms inside forms for some odd reason in the future then i am going to throw a monitor out the window.
	std::sort(MenuEntity->GetContainer().begin(), MenuEntity->GetContainer().end(),
	          [](child a, child b) { return b->GetLastClick() < a->GetLastClick(); }
	);
}

float LastOpen = 0;

void Render()
{
	if (IsKeyClicked(VK_INSERT) && LastOpen < clock() * 0.00001f)
	{
		LastOpen = (clock() * 0.00001f) + 0.002f;
		MenuOpen = !MenuOpen;
	}

	MenuEntity->Draw();
	MenuEntity->GetContainer()[0]->Update(); // only allow stretching,dragging and other update stuff if it is the main form, prevents dragging and sizing the wrong forms.
	SetFormPriority();
}
