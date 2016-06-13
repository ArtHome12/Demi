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


WindowsSettings::WindowsSettings(clan::Canvas &canvas, std::shared_ptr<SettingsStorage> &pSettingsStorage) : pSettings(pSettingsStorage)
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
	pLabelModelName = std::make_shared<clan::LabelView>();
	pLabelModelName->style()->set("flex: none;");
	pLabelModelName->style()->set("margin: 5px;");
	pLabelModelName->style()->set("font: 12px 'tahoma';");
	panelGeneral->add_child(pLabelModelName);

	// Панель с кнопками
	auto panelGeneral_panelButtons = std::make_shared<clan::View>();
	panelGeneral_panelButtons->style()->set("flex-direction: row");
	panelGeneral_panelButtons->style()->set("height: 32px");
	panelGeneral->add_child(panelGeneral_panelButtons);

	// Кнопки 
	//
	pButtonNew = Theme::create_button();
	pButtonNew->style()->set("width: 80px");
	pButtonNew->style()->set("margin-left: 5px");
	pButtonNew->image_view()->set_image(clan::Image(canvas, "New.png", pSettings->fileResDoc.get_file_system()));
	pButtonNew->image_view()->style()->set("padding-left: 3px");
	pButtonNew->label()->set_text("New");
	pButtonNew->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownNew);
	panelGeneral_panelButtons->add_child(pButtonNew);

	pButtonOpen = Theme::create_button();
	pButtonOpen->style()->set("width: 80px");
	pButtonOpen->style()->set("margin-left: 12px");
	pButtonOpen->image_view()->set_image(clan::Image(canvas, "Open.png", pSettings->fileResDoc.get_file_system()));
	pButtonOpen->image_view()->style()->set("padding-left: 3px");
	pButtonOpen->label()->set_text("Open");
	pButtonOpen->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownOpen);
	panelGeneral_panelButtons->add_child(pButtonOpen);

	auto bSave = Theme::create_button();
	bSave->style()->set("width: 80px");
	bSave->style()->set("margin-left: 12px");
	bSave->image_view()->set_image(clan::Image(canvas, "Save.png", pSettings->fileResDoc.get_file_system()));
	bSave->image_view()->style()->set("padding-left: 3px");
	bSave->label()->set_text("Save");
	bSave->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSave);
	panelGeneral_panelButtons->add_child(bSave);

	auto bSaveAs = Theme::create_button();
	bSaveAs->style()->set("width: 80px");
	bSaveAs->style()->set("margin-left: 12px");
	bSaveAs->image_view()->set_image(clan::Image(canvas, "SaveAs.png", pSettings->fileResDoc.get_file_system()));
	bSaveAs->image_view()->style()->set("padding-left: 3px");
	bSaveAs->label()->set_text("SaveAs");
	bSaveAs->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSaveAs);
	panelGeneral_panelButtons->add_child(bSaveAs);

	pButtonRestart = Theme::create_button();
	pButtonRestart->style()->set("width: 80px");
	pButtonRestart->style()->set("margin-left: 12px");
	pButtonRestart->image_view()->set_image(clan::Image(canvas, "Restart.png", pSettings->fileResDoc.get_file_system()));
	pButtonRestart->image_view()->style()->set("padding-left: 3px");
	pButtonRestart->label()->set_text("Restart");
	pButtonRestart->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRestart);
	panelGeneral_panelButtons->add_child(pButtonRestart);

	pButtonRunPause = Theme::create_button();
	pButtonRunPause->style()->set("width: 110px");
	pButtonRunPause->style()->set("margin-left: 12px");
	pButtonRunPause->set_sticky(true);
	pButtonRunPause->image_view()->set_image(clan::Image(canvas, "StartStop.png", pSettings->fileResDoc.get_file_system()));
	pButtonRunPause->image_view()->style()->set("padding-left: 3px");
	pButtonRunPause->label()->set_text("Run / Pause");
	pButtonRunPause->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRunPause);
	panelGeneral_panelButtons->add_child(pButtonRunPause);

	// Чекбокс для автозапуска модели
	pCBAutoRun = Theme::create_checkbox();
	pCBAutoRun->style()->set("margin: 12px 12px 6px;");
	pCBAutoRun->label()->set_text("Autostart last project on program load");
	pCBAutoRun->set_check(pSettings->getProjectAutorun());
	pCBAutoRun->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoRunToggle);
	panelGeneral->add_child(pCBAutoRun);

	// Чекбокс автосохранения модели при выходе из программы
	pCBAutoSave = Theme::create_checkbox();
	pCBAutoSave->style()->set("margin: 0px 12px;");
	pCBAutoSave->label()->set_text("Autosave project on program exit");
	pCBAutoSave->set_check(pSettings->getProjectAutosave());
	pCBAutoSave->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoSaveToggle);
	panelGeneral->add_child(pCBAutoSave);

	// Чекбокс периодического (ежечасного) автосохранения модели.
	pCBAutoSaveHourly = Theme::create_checkbox();
	pCBAutoSaveHourly->style()->set("margin: 6px 12px;");
	pCBAutoSaveHourly->label()->set_text("Autosave project every hour");
	pCBAutoSaveHourly->set_check(pSettings->getProjectAutosaveHourly());
	pCBAutoSaveHourly->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoSaveHourlyToggle);
	panelGeneral->add_child(pCBAutoSaveHourly);

	// Панель с информацией о модели
	auto panelModelInfo = std::make_shared<clan::View>();
	panelModelInfo->style()->set("flex-direction: column");
	panelModelInfo->style()->set("flex: auto");
	panelModelInfo->style()->set("border: 1px solid gray");
	add_child(panelModelInfo);

	// Если имя предудыщей модели отсутствует, создаём новую.
	const std::string lastModelFilename = pSettings->getProjectFilename();
	if (lastModelFilename == "") {
		onButtondownNew();
	}
	else {
		// Попытаемся загрузить последнюю модель.
		try {
			// Преобразуем путь в абсолютный, если у нас относительный.
			std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), lastModelFilename);
			globalEarth.LoadModel(absPath);
			set_modelFilename(lastModelFilename);
		}
		catch (const clan::Exception &e) {
			// При ошибке загружаем чистую модель, а текст ошибки добавляем к названию модели в качестве пояснения.
			onButtondownNew();
			pLabelModelName->set_text(pLabelModelName->text() + " (" + e.message + ")");
		}
	}
}

WindowsSettings::~WindowsSettings()
{
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
		clan::FileHelp::copy_file(cProjectTemplate, filename, true);
		set_modelFilename(filename);
		globalEarth.SaveModel(filename);
	}
}

void WindowsSettings::onButtondownRunPause()
{
	// Надо остановить или наоборот, запустить модель.
	bool toStart = pButtonRunPause->pressed();

	// Запускаем модель.
	globalEarth.RunEvolution(toStart);

	// Переключаем доступность кнопок - загрузить и создать можно только при остановленной модели.
	if (toStart) {
		pButtonNew->set_disabled();
		pButtonOpen->set_disabled();
		pButtonRestart->set_disabled();
	}
	else {
		pButtonNew->set_enabled();
		pButtonOpen->set_enabled();
		pButtonRestart->set_enabled();
	}
}

void WindowsSettings::onButtondownRestart()
{
	pCBAutoRun->set_disabled();
}

void WindowsSettings::onCBAutoRunToggle()
{
	pSettings->setProjectAutorun(pCBAutoRun->checked());
}

void WindowsSettings::onCBAutoSaveToggle()
{
	pSettings->setProjectAutosave(pCBAutoSave->checked());
}

void WindowsSettings::onCBAutoSaveHourlyToggle()
{
	pSettings->setProjectAutosaveHourly(pCBAutoSaveHourly->checked());
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

	// Сохраняем в настройках как последний открытый проект.
	pSettings->setProjectFilename(modelFilename);
}
