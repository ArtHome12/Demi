/* ===============================================================================
Моделирование эволюции живого мира.
Окно с настройками.
27 may 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "windows_settings.h"
#include "Theme/theme.h"




WindowsSettings::WindowsSettings(clan::Canvas &canvas)
{
	style()->set("background: lightgray");
	style()->set("flex: auto");
	style()->set("flex-direction: column");
	style()->set("border-top: 3px solid bisque");

	// Панель с общими настройками/инструментами
	//
	auto panelGeneral = std::make_shared<clan::View>();
	panelGeneral->style()->set("flex-direction: column");
	panelGeneral->style()->set("flex: auto");
	panelGeneral->style()->set("border: 1px solid gray");
	add_child(panelGeneral);

	// Название модели
	lModelName = std::make_shared<clan::LabelView>();
	lModelName->style()->set("flex: none");
	lModelName->style()->set("margin: 5px");
	lModelName->style()->set("font: 12px 'tahoma'");
	lModelName->set_text("Model name: Untitled");
	panelGeneral->add_child(lModelName);

	// Панель с кнопками
	auto panelGeneral_panelButtons = std::make_shared<clan::View>();
	panelGeneral_panelButtons->style()->set("flex-direction: row");
	panelGeneral_panelButtons->style()->set("height: 32px");
	panelGeneral->add_child(panelGeneral_panelButtons);

	// Кнопки 
	//
	auto bNew = Theme::create_button();
	bNew->style()->set("width: 80px");
	bNew->style()->set("margin-left: 5px");
	bNew->image_view()->set_image(clan::Image(canvas, "ThemeAero/New.png"));
	bNew->image_view()->style()->set("padding-left: 3px");
	bNew->label()->set_text("New");
	bNew->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownOpen);
	panelGeneral_panelButtons->add_child(bNew);

	auto bOpen = Theme::create_button();
	bOpen->style()->set("width: 80px");
	bOpen->style()->set("margin-left: 12px");
	bOpen->image_view()->set_image(clan::Image(canvas, "ThemeAero/Open.png"));
	bOpen->image_view()->style()->set("padding-left: 3px");
	bOpen->label()->set_text("Open");
	bOpen->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownOpen);
	panelGeneral_panelButtons->add_child(bOpen);

	auto bSave = Theme::create_button();
	bSave->style()->set("width: 80px");
	bSave->style()->set("margin-left: 12px");
	bSave->image_view()->set_image(clan::Image(canvas, "ThemeAero/Save.png"));
	bSave->image_view()->style()->set("padding-left: 3px");
	bSave->label()->set_text("Save");
	bSave->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSave);
	panelGeneral_panelButtons->add_child(bSave);

	auto bSaveAs = Theme::create_button();
	bSaveAs->style()->set("width: 80px");
	bSaveAs->style()->set("margin-left: 12px");
	bSaveAs->image_view()->set_image(clan::Image(canvas, "ThemeAero/SaveAs.png"));
	bSaveAs->image_view()->style()->set("padding-left: 3px");
	bSaveAs->label()->set_text("SaveAs");
	bSaveAs->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSaveAs);
	panelGeneral_panelButtons->add_child(bSaveAs);

	auto bRestart = Theme::create_button();
	bRestart->style()->set("width: 80px");
	bRestart->style()->set("margin-left: 12px");
	bRestart->image_view()->set_image(clan::Image(canvas, "ThemeAero/Restart.png"));
	bRestart->image_view()->style()->set("padding-left: 3px");
	bRestart->label()->set_text("Restart");
	bRestart->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRestart);
	panelGeneral_panelButtons->add_child(bRestart);

	auto bStartStop = Theme::create_button();
	bStartStop->style()->set("width: 110px");
	bStartStop->style()->set("margin-left: 12px");
	bStartStop->set_sticky(true);
	bStartStop->image_view()->set_image(clan::Image(canvas, "ThemeAero/StartStop.png"));
	bStartStop->image_view()->style()->set("padding-left: 3px");
	bStartStop->label()->set_text("Start / Stop");
	bStartStop->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownStartStop);
	panelGeneral_panelButtons->add_child(bStartStop);

	// Чекбокс для автозапуска модели
	auto cbAutoRun = Theme::create_checkbox();
	cbAutoRun->style()->set("margin: 12px");
	cbAutoRun->label()->set_text("Autostart last project on program load");
	panelGeneral->add_child(cbAutoRun);

	// Панель с информацией о модели
	//
	auto panelModelInfo = std::make_shared<clan::View>();
	panelModelInfo->style()->set("flex-direction: column");
	panelModelInfo->style()->set("flex: auto");
	panelModelInfo->style()->set("border: 1px solid gray");
	add_child(panelModelInfo);

}


// Обработчики событий
void WindowsSettings::onButtondownOpen()
{

}

void WindowsSettings::onButtondownSave()
{

}

void WindowsSettings::onButtondownSaveAs()
{

}

void WindowsSettings::onButtondownStartStop()
{

}

void WindowsSettings::onButtondownRestart()
{

}

