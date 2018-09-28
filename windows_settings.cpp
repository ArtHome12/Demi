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
#include <sstream>
#include "msg_boxes.h"

// Расширение xml-файла и двоичного файла модели и их описания.
auto cProjectXMLExtension = "demi";
auto cProjectXMLExtensionDesc = "Project files (*.demi)";
auto cProjectAllExtensionDesc = "All files (*.*)";

// Шаблон модели.
auto cProjectTemplate = "ThemeAero/template.demi";


// Строковые ресурсы
auto cWindowsSettingsDlgCaption = "WindowsSettingsDlgCaption";		// Заголовок для диалогов.
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
auto cWindowsSettingsAmountsLabel = "WindowsSettingsAmountsLabel";



WindowsSettings::WindowsSettings()
{
	// Настройки.
	auto pSettings = globalWorld.getSettingsStorage();

	style()->set("background: lightgray; height: 300px; flex-direction: column; border: 3px solid bisque");

	// Панель с общими настройками/инструментами (кнопки новый, сохранить и т.д., а также 3 чекбокса под ними.
	//
	auto panelGeneral = std::make_shared<clan::View>();
	panelGeneral->style()->set("flex-direction: column; height: 140px; border: 1px solid gray");
	add_child(panelGeneral);

	// Название модели
	pLabelModelName = std::make_shared<clan::LabelView>();
	pLabelModelName->style()->set("flex: none; margin: 5px; font: 12px 'tahoma'");
	panelGeneral->add_child(pLabelModelName);

	// Панель с кнопками
	auto panelGeneral_panelButtons = std::make_shared<clan::View>();
	panelGeneral_panelButtons->style()->set("flex-direction: row; height: 32px");
	panelGeneral->add_child(panelGeneral_panelButtons);

	// Кнопки 
	//
	pButtonNew = Theme::create_button();
	pButtonNew->style()->set("width: 120px; margin-left: 5px");
	pButtonNew->image_view()->style()->set("padding-left: 3px");
	pButtonNew->label()->set_text(pSettings->LocaleStr(cButtonLabelNew));
	panelGeneral_panelButtons->add_child(pButtonNew);

	pButtonOpen = Theme::create_button();
	pButtonOpen->style()->set("width: 120px; margin-left: 12px");
	pButtonOpen->image_view()->style()->set("padding-left: 3px");
	pButtonOpen->label()->set_text(pSettings->LocaleStr(cButtonLabelOpen));
	panelGeneral_panelButtons->add_child(pButtonOpen);

	pButtonSave = Theme::create_button();
	pButtonSave->style()->set("width: 120px; margin-left: 12px");
	pButtonSave->image_view()->style()->set("padding-left: 3px");
	pButtonSave->label()->set_text(pSettings->LocaleStr(cButtonLabelSave));
	panelGeneral_panelButtons->add_child(pButtonSave);

	pButtonSaveAs = Theme::create_button();
	pButtonSaveAs->style()->set("width: 120px; margin-left: 12px");
	pButtonSaveAs->image_view()->style()->set("padding-left: 3px");
	pButtonSaveAs->label()->set_text(pSettings->LocaleStr(cButtonLabelSaveAs));
	panelGeneral_panelButtons->add_child(pButtonSaveAs);

	pButtonRestart = Theme::create_button();
	pButtonRestart->style()->set("width: 120px; margin-left: 12px");
	pButtonRestart->image_view()->style()->set("padding-left: 3px");
	pButtonRestart->label()->set_text(pSettings->LocaleStr(cButtonLabelRestart));
	panelGeneral_panelButtons->add_child(pButtonRestart);

	pButtonRunPause = Theme::create_button();
	pButtonRunPause->style()->set("width: 150px; margin-left: 12px");
	pButtonRunPause->set_sticky(true);
	pButtonRunPause->image_view()->style()->set("padding-left: 3px");
	pButtonRunPause->label()->set_text(pSettings->LocaleStr(cButtonLabelRun));
	panelGeneral_panelButtons->add_child(pButtonRunPause);

	// Чекбокс для автозапуска модели
	pCBAutoRun = Theme::create_checkbox();
	pCBAutoRun->style()->set("margin: 12px 12px 6px;");
	pCBAutoRun->label()->set_text(pSettings->LocaleStr(cCBAutoRun));
	pCBAutoRun->set_check(pSettings->getProjectAutorun());
	panelGeneral->add_child(pCBAutoRun);

	// Чекбокс автосохранения модели при выходе из программы
	pCBAutoSave = Theme::create_checkbox();
	pCBAutoSave->style()->set("margin: 0px 12px;");
	pCBAutoSave->label()->set_text(pSettings->LocaleStr(cCBAutoSave));
	pCBAutoSave->set_check(pSettings->getProjectAutosave());
	panelGeneral->add_child(pCBAutoSave);

	// Чекбокс периодического (ежечасного) автосохранения модели.
	pCBAutoSaveHourly = Theme::create_checkbox();
	pCBAutoSaveHourly->style()->set("margin: 6px 12px;");
	pCBAutoSaveHourly->label()->set_text(pSettings->LocaleStr(cCBAutoSaveHourly));
	pCBAutoSaveHourly->set_check(pSettings->getProjectAutosaveHourly());
	panelGeneral->add_child(pCBAutoSaveHourly);

	// Панель с информацией о модели (чекбоксы видимости элементов, а также панель с количеством элементов).
	auto panelModelInfo = std::make_shared<clan::View>();
	panelModelInfo->style()->set("flex-direction: row; height: 160px");
	//panelModelInfo->style()->set("border: 1px solid red");
	add_child(panelModelInfo);

	// Дерево с галочками видимости элементов.
	pTreeView = std::make_shared<TreeView>();
	panelModelInfo->add_child(pTreeView);

	// Подпанель с количеством элементов правее дерева.
	panelElemAmounts = std::make_shared<clan::View>();
	panelElemAmounts->style()->set("flex-direction: column; background-color: lightgray; width: 210px");
	// panelElemAmounts->style()->set("border: 1px solid red");
	panelModelInfo->add_child(panelElemAmounts);
}

WindowsSettings::~WindowsSettings()
{
	// Если установлена галочка автосохранения, сделаем это.
	if (pCBAutoSave->checked())
		onButtondownSave();
}


// Завершающая часть настройки, которой требуется окно.
void WindowsSettings::initWindow(clan::Canvas& canvas)
{
	auto pSettings = globalWorld.getSettingsStorage();
	auto fs = pSettings->fileResDoc.get_file_system();

	pButtonNew->image_view()->set_image(clan::Image(canvas, "New.png", fs));
	pButtonNew->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownNew);
	pButtonOpen->image_view()->set_image(clan::Image(canvas, "Open.png", fs));
	pButtonOpen->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownOpen);
	pButtonSave->image_view()->set_image(clan::Image(canvas, "Save.png", fs));
	pButtonSave->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSave);
	pButtonSaveAs->image_view()->set_image(clan::Image(canvas, "SaveAs.png", fs));
	pButtonSaveAs->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownSaveAs);
	pButtonRestart->image_view()->set_image(clan::Image(canvas, "Restart.png", fs));
	pButtonRestart->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRestart);
	pButtonRunPause->image_view()->set_image(clan::Image(canvas, "StartStop.png", fs));
	pButtonRunPause->func_clicked() = clan::bind_member(this, &WindowsSettings::onButtondownRunPause);

	pCBAutoRun->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoRunToggle);
	pCBAutoSave->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoSaveToggle);
	pCBAutoSaveHourly->func_state_changed() = clan::bind_member(this, &WindowsSettings::onCBAutoSaveHourlyToggle);
}


// Для первоначальной загрузки модели.
void WindowsSettings::finishInit(clan::WindowManager* windowManager)
{
	auto pSettings = globalWorld.getSettingsStorage();

	wManager = windowManager;

	// Если имя предыдущей модели отсутствует, создаём новую.
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
	if (modelFilename.empty())
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

			auto pSettings = globalWorld.getSettingsStorage();
			const std::string caption(pSettings->LocaleStr(cWindowsSettingsDlgCaption));
			const std::string text(clan::string_format(pSettings->LocaleStr(cMessageBoxTextFileRewrite), filename));
			auto dialog = std::make_shared<MsgBox>(pSettings, text, caption, cMbOkCancel);
			dialog->loadIcons(canvas(), pSettings->fileResDoc.get_file_system());

			dialog->onProcessResult = [=](eMbResultType result)
			{
				if (result == cMbResultOk) {
					// Запоминаем новое имя проекта и перезаписываем XML-файл, сохраняем двоичный файл.
					clan::FileHelp::copy_file(cProjectTemplate, filename, true);
					set_modelFilename(filename);
					globalWorld.saveModel(filename);

				}
			};
			wManager->present_modal(this, dialog);
			
			// Перезапись будет сделана в колбек-функции.
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
	// Начать расчёт заново. Получим подтверждение пользователя.
	//
	auto pSettings = globalWorld.getSettingsStorage();
	const std::string caption(pSettings->LocaleStr(cWindowsSettingsDlgCaption));
	const std::string text(pSettings->LocaleStr(cMessageBoxTextRestartModel));
	auto dialog = std::make_shared<MsgBox>(pSettings, text, caption, cMbOkCancel);
	dialog->loadIcons(canvas(), pSettings->fileResDoc.get_file_system());

	dialog->onProcessResult = [=](eMbResultType result)
	{
		if (result == cMbResultOk) {
			// Преобразуем путь в абсолютный, если у нас относительный.
			std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), modelFilename);

			// Сбрасываем модель.
			globalWorld.resetModel(absPath, cProjectTemplate);

			// Для новой модели применяем текущие настройки видимости.
			initElemVisibilityTreeAfterRestart();
		}
	};
	wManager->present_modal(this, dialog);
}

void WindowsSettings::onCBAutoRunToggle()
{
	auto pSettings = globalWorld.getSettingsStorage();
	pSettings->setProjectAutorun(pCBAutoRun->checked());
}

void WindowsSettings::onCBAutoSaveToggle()
{
	auto pSettings = globalWorld.getSettingsStorage();
	pSettings->setProjectAutosave(pCBAutoSave->checked());
}

void WindowsSettings::onCBAutoSaveHourlyToggle()
{
	auto pSettings = globalWorld.getSettingsStorage();
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
		spec->setVisible(item.checked);
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
		modelFilename.clear();

	// Информация для пользователя.
	auto pSettings = globalWorld.getSettingsStorage();
	pLabelModelName->set_text(pSettings->LocaleStr(cLabelModelName) + (!modelFilename.empty() ? modelFilename : pSettings->LocaleStr(cLabelModelAbsent)));

	// Сохраняем в настройках как последний открытый проект.
	pSettings->setProjectFilename(modelFilename);
}

// Для получения уведомлений об отработке основного цикла (для возможного автосохранения модели, обновления количеств).
void WindowsSettings::modelRenderNotify(size_t secondsElapsed)
{
	// Обновим надписи с количествами элементов согласно информации из специального объекта.
	// Пользуемся тем, что знаем порядок - первым идёт надпись напротив "неживая природа", далее первый элемент и т.д.
	// Так как set_text с использованием force_no_layout не проверяет, свёрнуто ли окно, сделаем это предварительно.
	if (!hidden()) {
		const size_t cnt = globalWorld.getElemCount();
		auto child = ++panelElemAmounts->children().begin();
		for (size_t i = 0; i < cnt; i++) {

			// Преобразуем указатель на базовый тип View на дочерний LabelView.
			std::shared_ptr<clan::LabelView> label = std::dynamic_pointer_cast<clan::LabelView>(*child++);

			// Впишем количество с делением по разрядам.
			std::string str = IntToStrWithDigitPlaces<unsigned long long>(globalWorld.amounts.getResAmounts(i));

			label->set_text(str, true);
		}
	}

	// Если прошёл час и установлена галочка регулярного автосохранения модели, сделаем это.
	if (secondsElapsed - this->secondsElapsed > 3600) {
		this->secondsElapsed = secondsElapsed;

		// Если стоит галочка, сохраним модель.
		if (pCBAutoSaveHourly->checked())
			onButtondownSave();
	}
}


// Обновляет дерево с галочками видимости элементов.
void WindowsSettings::initElemVisibilityTree()
{
	// Удалим старые метки под количество элементов, если они были. Обращаемся по индексу, иначе итератор портится.
	for (int i = panelElemAmounts->children().size(); i>0; --i) 
		panelElemAmounts->children().at(i-1)->remove_from_parent();

	auto rootNode = std::make_shared<TreeItem>("", 0);	// В tag хранится либо индекс химэлемента, если первый символ имени - пробел, либо указатель на организм. 0 - заглушка.
	
	// Первый узел - под химические элементы.
	auto pSettings = globalWorld.getSettingsStorage();
	auto firstNode = std::make_shared<TreeItem>(pSettings->LocaleStr(cTreeInanimate), 0);

	// Добавляем сразу и метку в панель количества.
	panelElemAmounts->add_child(createLabelForAmount(pSettings->LocaleStr(cWindowsSettingsAmountsLabel)));

	// Добавляем химэлементы, первым символом пробел для маркировки, что это неживой элемент.
	for (size_t i = 0; i != globalWorld.getElemCount(); ++i) {
		// Чекбокс с названием элемента.
		firstNode->children.push_back(std::make_shared<TreeItem>(" " + globalWorld.getResName(i), i, globalWorld.getResVisibility(i)));

		// Метка под количество.
		panelElemAmounts->add_child(createLabelForAmount("0"));
	}

	rootNode->children.push_back(firstNode);

	// Протоорганизм (вид).
	auto luca = globalWorld.getSpecies();

	// Корневой узел под организмы.
	auto curNode = std::make_shared<TreeItem>(pSettings->LocaleStr(cTreeAnimate) + ": " + luca->getName() + " (" + luca->getAuthor() + ")", size_t(luca.get()), luca->getVisible());
	rootNode->children.push_back(curNode);

	// Добавляем виды организмов, удаляя пробелы в начале на всякий случай (чтобы не спутать с неживой природой). В качестве tag - указатель.

	// Устанавливаем обработчик событий на переключение галочек.
	pTreeView->func_check_changed() = clan::bind_member(this, &WindowsSettings::onTreeCheckChanged);

	// Добавляем дерево в список.
	pTreeView->set_root_item(rootNode);
}

// Есть особенности по сравнению с обычной загрузкой.
void WindowsSettings::initElemVisibilityTreeAfterRestart()
{
	// Корневой узел.
	std::shared_ptr<TreeItem> rootNode = pTreeView->get_root_item();

	// Для неживой природы просто обновим значения.
	auto inanimalNode = rootNode->children.at(0);
	for (auto &child : inanimalNode->children) 
		globalWorld.setResVisibility(child->tag, child->checked);

	// Для организмов надо обновить указатели в поле tag.
	// Пока недоделано - всего один организм.

	// Протоорганизм (вид).
	auto luca = globalWorld.getSpecies();

	auto firstAnimalNode = rootNode->children.at(1);
	firstAnimalNode->tag = size_t(luca.get());

	// Обновим видимость самого организма на основе значения чекбокса.
	luca->setVisible(firstAnimalNode->checked);
}


// Создаёт надпись.
std::shared_ptr<clan::LabelView> WindowsSettings::createLabelForAmount(std::string text)
{
	auto retVal = std::make_shared<clan::LabelView>();
	retVal->style()->set("font: 13px 'Segoe UI'; color: black; height: 17px");
	//retVal->set_text_alignment(clan::TextAlignment::right);
	retVal->set_text(text);
	return retVal;
}



// trim from beginning of string (left)
inline std::string& ltrim(std::string& s)
{
	s.erase(0, s.find_first_not_of(' '));
	return s;
}