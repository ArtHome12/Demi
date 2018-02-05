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
#include "world.h"
#include <clocale>

// Расширение xml-файла и двоичного файла модели и их описания.
auto cProjectXMLExtension = "demi";
auto cProjectXMLExtensionDesc = "Project files (*.demi)";
auto cProjectAllExtensionDesc = "All files (*.*)";

// Шаблон модели.
auto cProjectTemplate = "ThemeAero/template.demi";


// Строковые ресурсы
auto cMessageBoxTextFileRewrite = "WindowsSettingsFileExistDlg";	// Строка для запроса перезаписи файла.
auto cMessageBoxTextRestartModel = "WindowsSettingsRestartModel";	// Строка для запроса рестарта модели.
auto cButtonLabelNew = "WindowsSettingsButtonNew";
auto cButtonLabelOpen = "WindowsSettingsButtonOpen";
auto cButtonLabelSave = "WindowsSettingsButtonSave";
auto cButtonLabelSaveAs = "WindowsSettingsButtonSaveAs";
auto cButtonLabelRestart = "WindowsSettingsButtonRestart";
auto cButtonLabelRun = "WindowsSettingsButtonRun";
auto cCBAutoRun = "WindowsSettingsCheckboxAutoRun";
auto cCBAutoSave = "WindowsSettingsCheckboxAutoSave";
auto cCBAutoSaveHourly = "WindowsSettingsCheckboxAutoSaveHourly";
auto cLabelModelName = "WindowsSettingsProjectName";
auto cLabelModelAbsent = "WindowsSettingsProjectNameAbsent";
// Для дерева
auto cTreeInanimate = "WindowsSettingsTreeInanimateSubTree";
auto cTreeAnimate = "WindowsSettingsTreeAnimateSubTree";




WindowsSettings::WindowsSettings(clan::Canvas &canvas, std::shared_ptr<SettingsStorage> pSettingsStorage) : pSettings(pSettingsStorage)
{
	style()->set("background: lightgray");
	style()->set("height: 300px");
	style()->set("flex-direction: column");
	style()->set("border-top: 3px solid bisque");

	// Панель с общими настройками/инструментами
	//
	auto panelGeneral = std::make_shared<clan::View>();
	panelGeneral->style()->set("flex-direction: column");
	panelGeneral->style()->set("height: 140px");
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
	pButtonNew->style()->set("width: 120px");
	pButtonNew->style()->set("margin-left: 5px");
	pButtonNew->image_view()->set_image(clan::Image(canvas, "New.png", pSettings->fileResDoc.get_file_system()));
	pButtonNew->image_view()->style()->set("padding-left: 3px");
	pButtonNew->label()->set_text(pSettings->LocaleStr(cButtonLabelNew));
	pButtonNew->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownNew);
	panelGeneral_panelButtons->add_child(pButtonNew);

	pButtonOpen = Theme::create_button();
	pButtonOpen->style()->set("width: 120px");
	pButtonOpen->style()->set("margin-left: 12px");
	pButtonOpen->image_view()->set_image(clan::Image(canvas, "Open.png", pSettings->fileResDoc.get_file_system()));
	pButtonOpen->image_view()->style()->set("padding-left: 3px");
	pButtonOpen->label()->set_text(pSettings->LocaleStr(cButtonLabelOpen));
	pButtonOpen->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownOpen);
	panelGeneral_panelButtons->add_child(pButtonOpen);

	auto bSave = Theme::create_button();
	bSave->style()->set("width: 120px");
	bSave->style()->set("margin-left: 12px");
	bSave->image_view()->set_image(clan::Image(canvas, "Save.png", pSettings->fileResDoc.get_file_system()));
	bSave->image_view()->style()->set("padding-left: 3px");
	bSave->label()->set_text(pSettings->LocaleStr(cButtonLabelSave));
	bSave->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSave);
	panelGeneral_panelButtons->add_child(bSave);

	auto bSaveAs = Theme::create_button();
	bSaveAs->style()->set("width: 120px");
	bSaveAs->style()->set("margin-left: 12px");
	bSaveAs->image_view()->set_image(clan::Image(canvas, "SaveAs.png", pSettings->fileResDoc.get_file_system()));
	bSaveAs->image_view()->style()->set("padding-left: 3px");
	bSaveAs->label()->set_text(pSettings->LocaleStr(cButtonLabelSaveAs));
	bSaveAs->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSaveAs);
	panelGeneral_panelButtons->add_child(bSaveAs);

	pButtonRestart = Theme::create_button();
	pButtonRestart->style()->set("width: 120px");
	pButtonRestart->style()->set("margin-left: 12px");
	pButtonRestart->image_view()->set_image(clan::Image(canvas, "Restart.png", pSettings->fileResDoc.get_file_system()));
	pButtonRestart->image_view()->style()->set("padding-left: 3px");
	pButtonRestart->label()->set_text(pSettings->LocaleStr(cButtonLabelRestart));
	pButtonRestart->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRestart);
	panelGeneral_panelButtons->add_child(pButtonRestart);

	pButtonRunPause = Theme::create_button();
	pButtonRunPause->style()->set("width: 150px");
	pButtonRunPause->style()->set("margin-left: 12px");
	pButtonRunPause->set_sticky(true);
	pButtonRunPause->image_view()->set_image(clan::Image(canvas, "StartStop.png", pSettings->fileResDoc.get_file_system()));
	pButtonRunPause->image_view()->style()->set("padding-left: 3px");
	pButtonRunPause->label()->set_text(pSettings->LocaleStr(cButtonLabelRun));
	pButtonRunPause->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRunPause);
	panelGeneral_panelButtons->add_child(pButtonRunPause);

	// Чекбокс для автозапуска модели
	pCBAutoRun = Theme::create_checkbox();
	pCBAutoRun->style()->set("margin: 12px 12px 6px;");
	pCBAutoRun->label()->set_text(pSettings->LocaleStr(cCBAutoRun));
	pCBAutoRun->set_check(pSettings->getProjectAutorun());
	pCBAutoRun->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoRunToggle);
	panelGeneral->add_child(pCBAutoRun);

	// Чекбокс автосохранения модели при выходе из программы
	pCBAutoSave = Theme::create_checkbox();
	pCBAutoSave->style()->set("margin: 0px 12px;");
	pCBAutoSave->label()->set_text(pSettings->LocaleStr(cCBAutoSave));
	pCBAutoSave->set_check(pSettings->getProjectAutosave());
	pCBAutoSave->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoSaveToggle);
	panelGeneral->add_child(pCBAutoSave);

	// Чекбокс периодического (ежечасного) автосохранения модели.
	pCBAutoSaveHourly = Theme::create_checkbox();
	pCBAutoSaveHourly->style()->set("margin: 6px 12px;");
	pCBAutoSaveHourly->label()->set_text(pSettings->LocaleStr(cCBAutoSaveHourly));
	pCBAutoSaveHourly->set_check(pSettings->getProjectAutosaveHourly());
	pCBAutoSaveHourly->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoSaveHourlyToggle);
	panelGeneral->add_child(pCBAutoSaveHourly);

	// Панель с информацией о модели
	auto panelModelInfo = std::make_shared<clan::View>();
	panelModelInfo->style()->set("flex-direction: column");
	panelModelInfo->style()->set("height: 160px");
	//panelModelInfo->style()->set("border: 1px solid gray");
	add_child(panelModelInfo);

	// Дерево с галочками видимости элементов.
	pTreeView = std::make_shared<TreeView>();
	//pTreeView->style()->set("flex: auto; border: 1px solid red");
	panelModelInfo->add_child(pTreeView);

	// Если имя предудыщей модели отсутствует, создаём новую.
	auto lastModelFilename = pSettings->getProjectFilename();
	if (lastModelFilename == "") {
		onButtondownNew();
	}
	else {
		// Попытаемся загрузить последнюю модель.
		try {
			// Преобразуем путь в абсолютный, если у нас относительный.
			std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), lastModelFilename);
			globalWorld.loadModel(absPath);
			set_modelFilename(lastModelFilename);

			// Обновим дерево с галочками видимости элементов.
			initElemVisibilityTree();
		}
		catch (const clan::Exception &e) {
			// При ошибке загружаем чистую модель, а текст ошибки добавляем к названию модели в качестве пояснения.
			onButtondownNew();
			pLabelModelName->set_text(pLabelModelName->text() + " (" + e.message + ")");
		}
	}

	// Если установлена галочка автозапуска модели, запустим расчёт.
	if (pCBAutoRun->checked()) {
		pButtonRunPause->set_pressed(true);
		onButtondownRunPause();
	}
}

WindowsSettings::~WindowsSettings()
{
	// Если установлена галочка автосохранения, сделаем это.
	if (pCBAutoSave->checked())
		onButtondownSave();
}


// Обработчики событий
void WindowsSettings::onButtondownNew()
{
	// Сбросим имя модели и загрузим в модель шаблон.
	set_modelFilename("");
	globalWorld.loadModel(cProjectTemplate);

	// Обновим дерево с галочками видимости элементов.
	initElemVisibilityTree();
}

void WindowsSettings::onButtondownOpen()
{
	// Создаём и инициализируем диалог выбора имени файла.
	//
	auto dlg = std::make_shared<clan::OpenFileDialog>(this);
	dlg->add_filter(cProjectXMLExtensionDesc, "*." + std::string(cProjectXMLExtension), true);
	dlg->add_filter(cProjectAllExtensionDesc, "*", false);
	if (dlg->show()) {
		// Сохраним имя модели и загрузим её.
		set_modelFilename(dlg->filename());
		globalWorld.loadModel(dlg->filename());

		// Обновим дерево с галочками видимости элементов.
		initElemVisibilityTree();
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
		globalWorld.saveModel(absPath);
	}
}


void WindowsSettings::onButtondownSaveAs()
{
	// Создаём и инициализируем диалог выбора имени файла.
	//
	auto dlg = std::make_shared<clan::SaveFileDialog>(this);
	dlg->add_filter(cProjectXMLExtensionDesc, "*." + std::string(cProjectXMLExtension), true);
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
			auto text = SettingsStorage::UTF8_to_CP1251(std::string(clan::string_format(pSettings->LocaleStr(cMessageBoxTextFileRewrite), filename)));
			if (MessageBox(window.get_handle().hwnd, text.c_str(), SettingsStorage::UTF8_to_CP1251(window.get_title()).c_str(), MB_OKCANCEL) != IDOK)
				// Пользователь отказался продолжать, выходим.
				return;
		}

		// Запоминаем новое имя проекта и перезаписываем XML-файл, сохраняем двоичный файл.
		clan::FileHelp::copy_file(cProjectTemplate, filename, true);
		set_modelFilename(filename);
		globalWorld.saveModel(filename);
	}
}

void WindowsSettings::onButtondownRunPause()
{
	// Надо остановить или наоборот, запустить модель.
	bool toStart = pButtonRunPause->pressed();

	// Запускаем модель.
	globalWorld.runEvolution(toStart);

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
	// Начать расчёт заново.

	// Получим подтверждение пользователя. Надо переделать на платформонезависимое решение!
	const clan::DisplayWindow &window = view_tree()->display_window();
	auto text = SettingsStorage::UTF8_to_CP1251(pSettings->LocaleStr(cMessageBoxTextRestartModel));
	if (MessageBox(window.get_handle().hwnd, text.c_str(), SettingsStorage::UTF8_to_CP1251(window.get_title()).c_str(), MB_OKCANCEL) != IDOK)
		// Пользователь отказался продолжать, выходим.
		return;

	// Преобразуем путь в абсолютный, если у нас относительный.
	std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), modelFilename);

	// Сбрасываем модель.
	globalWorld.resetModel(absPath, cProjectTemplate);
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


// Обработчик события, вызываемый после переключения галочки на элементе древовидного списка.
void WindowsSettings::onTreeCheckChanged(TreeItem &item)
{
	// Сообщим об изменении видимости элемента модели, если узел соответствует какому-либо элементу. Если первым символом идёт пробел, то это неживая природа.
	if (item.caption.substr(0, 1) == " ")
		globalWorld.setResVisibility(item.tag, item.checked);
	// Ещё это может быть техническая надпись, например "Неживая природа".
	else if (item.tag != 0) {
		// Считаем, что это указатель на вид организма.
		demi::Species *spec = reinterpret_cast<demi::Species *>(item.tag);
		spec->set_visible(item.checked);
	}
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
	pLabelModelName->set_text(pSettings->LocaleStr(cLabelModelName) + (modelFilename != "" ? modelFilename : pSettings->LocaleStr(cLabelModelAbsent)));

	// Сохраняем в настройках как последний открытый проект.
	pSettings->setProjectFilename(modelFilename);
}

void WindowsSettings::modelRenderNotify(float secondsElapsed)
{
	// Для получения уведомлений об отработке основного цикла (для возможного автосохранения модели).

	// Если прошёл час и установлена галочка регулярного автосохранения модели, сделаем это.
	if (abs(secondsElapsed - this->secondsElapsed) > 3600.0f) {
		this->secondsElapsed = secondsElapsed;

		// Если стоит галочка, сохраним модель.
		if (pCBAutoSaveHourly->checked())
			onButtondownSave();
	}
}


// Обновляет дерево с галочками видимости элементов.
void WindowsSettings::initElemVisibilityTree()
{
	auto rootNode = std::make_shared<TreeItem>("", -1);
	
	// Первый узел - под химические элементы.
	auto firstNode = std::make_shared<TreeItem>(pSettings->LocaleStr(cTreeInanimate), -1);

	// Добавляем их, первым символом пробел для маркировки, что это неживой элемент.
	for (int i = 0; i < globalWorld.getElemCount(); ++i)
		firstNode->children.push_back(std::make_shared<TreeItem>(" " + globalWorld.getResName(i), i, globalWorld.getResVisibility(i)));

	rootNode->children.push_back(firstNode);

	// Протоорганизм (вид).
	auto luca = globalWorld.getSpecies();

	// Корневой узел под организмы.
	auto curNode = std::make_shared<TreeItem>(pSettings->LocaleStr(cTreeAnimate) + ": " + luca->name + " (" + luca->author + ")", int(luca.get()), luca->visible);
	rootNode->children.push_back(curNode);

	// Добавляем виды организмов, удаляя пробелы в начале на всякий случай (чтобы не спутать с неживой природой). В качестве tag - указатель.

	// Устанавливаем обработчик событий на переключение галочек.
	pTreeView->func_check_changed() = clan::bind_member(this, &WindowsSettings::onTreeCheckChanged);

	// Добавляем дерево в список.
	pTreeView->set_root_item(rootNode);
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s)
{
	s.erase(0, s.find_first_not_of(" "));
	return s;
}