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
#include "earth.h"

// Расширение xml-файла и двоичного файла модели и их описания.
const std::string cProjectXMLExtension = "demi";
const std::string cProjectBinExtension = "demib";
const std::string cProjectXMLExtensionDesc = "Project files (*.demi)";
const std::string cProjectAllExtensionDesc = "All files (*.*)";

// Шаблон модели.
const std::string cProjectTemplate = "ThemeAero/template.demi";


// Строки для запроса перезаписи.
const std::string cMessageBoxText = "File %1 exists. Rewrite it?";


WindowsSettings::WindowsSettings(clan::Canvas &canvas)
{
	// Настройки программы
	pSettings = std::make_shared<SettingsStorage>();

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
	pLabelModelName = std::make_shared<clan::LabelView>();
	pLabelModelName->style()->set("flex: none;");
	pLabelModelName->style()->set("margin: 5px;");
	pLabelModelName->style()->set("font: 12px 'tahoma';");
	set_modelFilename(pSettings->getProjectFilename());
	panelGeneral->add_child(pLabelModelName);

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
	bNew->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownNew);
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

	pButtonRunPause = Theme::create_button();
	pButtonRunPause->style()->set("width: 110px");
	pButtonRunPause->style()->set("margin-left: 12px");
	pButtonRunPause->set_sticky(true);
	pButtonRunPause->image_view()->set_image(clan::Image(canvas, "ThemeAero/StartStop.png"));
	pButtonRunPause->image_view()->style()->set("padding-left: 3px");
	pButtonRunPause->label()->set_text("Run / Pause");
	pButtonRunPause->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRunPause);
	panelGeneral_panelButtons->add_child(pButtonRunPause);

	// Чекбокс для автозапуска модели
	pCBAutoRun = Theme::create_checkbox();
	pCBAutoRun->style()->set("margin: 12px 12px 6px;");
	pCBAutoRun->label()->set_text("Autostart last project on program load");
	pCBAutoRun->set_check(pSettings->getProjectAutorun());
	panelGeneral->add_child(pCBAutoRun);

	// Чекбокс автосохранения модели при выходе из программы
	pCBAutoSave = Theme::create_checkbox();
	pCBAutoSave->style()->set("margin: 6px 12px;");
	pCBAutoSave->label()->set_text("Autosave project on program exit");
	pCBAutoSave->set_check(pSettings->getProjectAutosave());
	panelGeneral->add_child(pCBAutoSave);

	// Чекбокс периодического (ежечасного) автосохранения модели.
	pCBAutoSaveHourly = Theme::create_checkbox();
	pCBAutoSaveHourly->style()->set("margin: 0px 12px;");
	pCBAutoSaveHourly->label()->set_text("Autosave project every hour");
	pCBAutoSaveHourly->set_check(pSettings->getProjectAutosaveHourly());
	panelGeneral->add_child(pCBAutoSaveHourly);

	// Панель с информацией о модели
	//
	auto panelModelInfo = std::make_shared<clan::View>();
	panelModelInfo->style()->set("flex-direction: column");
	panelModelInfo->style()->set("flex: auto");
	panelModelInfo->style()->set("border: 1px solid gray");
	add_child(panelModelInfo);

	// Загрузим пустую модель.
	onButtondownNew();
}

WindowsSettings::~WindowsSettings()
{
	pSettings->setProjectInfo(modelFilename, pCBAutoRun->checked(), pCBAutoSave->checked(), pCBAutoSaveHourly->checked());
}


// Обработчики событий
void WindowsSettings::onButtondownNew()
{
	// Сбросим имя модели и загрузим в модель шаблон.
	set_modelFilename("");
	globalEarth.LoadModel(cProjectTemplate);
}

void WindowsSettings::onButtondownOpen()
{
	// Создаём и инициализируем диалог выбора имени файла.
	//
	auto dlg = std::make_shared<clan::OpenFileDialog>(this);
	dlg->add_filter(cProjectXMLExtensionDesc, "*." + cProjectXMLExtension, true);
	dlg->add_filter(cProjectAllExtensionDesc, "*", false);
	if (dlg->show()) {
		// Сохраним имя модели и загрузим её.
		set_modelFilename(dlg->filename());
		globalEarth.LoadModel(dlg->filename());
	}
}

void WindowsSettings::onButtondownSave()
{
	// Если модель не была ранее сохранена, вызовем "сохранить как".
	if (modelFilename == "")
		onButtondownSaveAs();
	else {
		// Преобразуем путь в абсолютный, если у нас относительный.
		std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), modelFilename);
		globalEarth.SaveModel(absPath);
	}
}

void WindowsSettings::onButtondownSaveAs()
{
	// Создаём и инициализируем диалог выбора имени файла.
	//
	auto dlg = std::make_shared<clan::SaveFileDialog>(this);
	dlg->add_filter(cProjectXMLExtensionDesc, "*." + cProjectXMLExtension, true);
	dlg->add_filter(cProjectAllExtensionDesc, "*", false);
	
	if (dlg->show()) {
		
		// Введённое или выбранное пользователем имя файла.
		std::string filename = dlg->filename();

		// Добавим расширение, если его нет.
		std::string & ext = clan::PathHelp::get_extension(filename);
		if (ext != cProjectXMLExtension)
			filename = filename + "." + cProjectXMLExtension;

		// Если файл существует, необходимо спросить о перезаписи.
		if (clan::FileHelp::file_exists(filename)) {
			// Надо переделать на платформонезависимое решение!
			const clan::DisplayWindow &window = view_tree()->display_window();
			auto text = std::string(clan::string_format(cMessageBoxText, filename));
			if (MessageBox(window.get_handle().hwnd, text.c_str(), window.get_title().c_str(), MB_OKCANCEL) != IDOK)
				// Пользователь отказался продолжать, выходим.
				return;
		}

		// Запоминаем новое имя проекта и перезаписываем XML-файл, сохраняем двоичный файл.
		set_modelFilename(filename);
		clan::FileHelp::copy_file(cProjectTemplate, filename, true);
		globalEarth.SaveModel(filename);
	}
}

void WindowsSettings::onButtondownRunPause()
{
	globalEarth.RunEvolution(pButtonRunPause->pressed());
}

void WindowsSettings::onButtondownRestart()
{
	pCBAutoRun->set_disabled();
}

// Сохраняет новое имя модели и обновляет надпись на экране.
void WindowsSettings::set_modelFilename(const std::string &newName)
{
	// Преобразуем путь в абсолютный, если у нас относительный.
	std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), newName);
	
	// Проверяем, если файл недоступен, сбрасываем имя.
	if (clan::FileHelp::file_exists(absPath))
		// Сохраняем по-возможности относительный путь для облегчения копирования и переноса программы.
		modelFilename = clan::PathHelp::make_relative(clan::System::get_exe_path(), absPath);
	else
		modelFilename = "";

	// Информация для пользователя.
	pLabelModelName->set_text("Model name: " + (modelFilename != "" ? modelFilename : "not selected or not available"));
}
