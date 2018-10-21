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
auto cWindowsSettingsDlgSavingModel = "WindowsSettingsDlgSavingModel";
auto cWindowsSettingsDlgLoadingModel = "WindowsSettingsDlgLoadingModel";
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

	style()->set("background: lightgray; flex-direction: column; border: 3px solid bisque");

	// Панель с общими настройками/инструментами (кнопки новый, сохранить и т.д., а также 3 чекбокса под ними.
	//
	auto panelGeneral = std::make_shared<clan::View>();
	panelGeneral->style()->set("flex-direction: column; height: 140px");
	panelGeneral->style()->set("border-top-color: gray; border-top-style: solid; border-top-width: 1px");
	panelGeneral->style()->set("border-bottom-color: gray; border-bottom-style: solid; border-bottom-width: 1px");

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
	panelModelInfo->style()->set("flex-direction: row");
	//panelModelInfo->style()->set("border: 1px solid red");
	add_child(panelModelInfo);

	// Дерево с галочками видимости элементов.
	pTreeViewElements = std::make_shared<TreeView>();
	pTreeViewElements->func_check_changed = clan::bind_member(this, &WindowsSettings::onTreeElementsCheckChanged);	// Обработчик переключения галочек.
	panelModelInfo->add_child(pTreeViewElements);

	// Подпанель с количеством элементов правее дерева.
	panelElemAmounts = std::make_shared<clan::View>();
	// Граница справа появится только после исправления в clanlib, issue #104
	panelElemAmounts->style()->set("flex-direction: column; background-color: lightgray; width: 210px; border-right-width: 1px; border-right-style: solid; border-right-color: green");
	//panelElemAmounts->style()->set("border: 1px solid red");
	panelModelInfo->add_child(panelElemAmounts);

	// Разделитель

	// Дерево с галочками видимости видов.
	pTreeViewSpecies = std::make_shared<TreeView>();
	pTreeViewSpecies->func_check_changed = clan::bind_member(this, &WindowsSettings::onTreeSpeciesCheckChanged);
	panelModelInfo->add_child(pTreeViewSpecies);
	panelOrganismAmounts = std::make_shared<clan::View>();
	panelOrganismAmounts->style()->set("flex-direction: column; background-color: lightgray; width: 210px; border-right-width: 1px; border-right-style: solid; border-right-color: green");
	panelModelInfo->add_child(panelOrganismAmounts);

	// Кешированный префикс для надписи для живых организмов, для ускорения.
	cachedAnimalPrefixLabel = pSettings->LocaleStr(cTreeAnimate) + ": ";
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
			loadModel(absPath);
			setModelFilename(lastModelFilename);

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
	setModelFilename("");
	loadModel(cProjectTemplate);

	// Обновим дерево с галочками видимости элементов.
	initElemVisibilityTree();

	// Для обновления списка видов при первой возможности.
	globalWorld.genotypesTree.flagSpaciesChanged = true;
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
		setModelFilename(dlg->filename());
		loadModel(dlg->filename());

		// Обновим дерево с галочками видимости элементов.
		initElemVisibilityTree();

		// Для обновления списка видов при первой возможности.
		globalWorld.genotypesTree.flagSpaciesChanged = true;
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
		saveModel(absPath);
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

			dialog->onProcessResult = [=](eMbResultType result)
			{
				if (result == cMbResultOk) {
					// Запоминаем новое имя проекта и перезаписываем XML-файл, сохраняем двоичный файл.
					clan::FileHelp::copy_file(cProjectTemplate, filename, true);
					setModelFilename(filename);
					saveModel(filename);

				}
			};
			wManager->present_modal(this, dialog);
			dialog->initWindow(pSettings->fileResDoc.get_file_system());

			// Перезапись будет сделана в колбек-функции.
			return;
		}

		// Запоминаем новое имя проекта и перезаписываем XML-файл, сохраняем двоичный файл.
		clan::FileHelp::copy_file(cProjectTemplate, filename, true);
		setModelFilename(filename);
		saveModel(filename);
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

	dialog->onProcessResult = [=](eMbResultType result)
	{
		if (result == cMbResultOk) {
			// Преобразуем путь в абсолютный, если у нас относительный.
			std::string absPath = clan::PathHelp::make_absolute(clan::System::get_exe_path(), modelFilename);

			// Сбрасываем модель.
			globalWorld.resetModel(absPath, cProjectTemplate);

			// Для новой модели применяем текущие настройки видимости.
			initElemVisibilityTreeAfterRestart();

			// Для обновления списка видов при первой возможности.
			globalWorld.genotypesTree.flagSpaciesChanged = true;
		}
	};
	wManager->present_modal(this, dialog);
	dialog->initWindow(pSettings->fileResDoc.get_file_system());
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
void WindowsSettings::onTreeElementsCheckChanged(TreeItem &item)
{
	// Если тег не задан, это корневой элемент и надо переключить дочерние.
	if (item.tag == size_t(SIZE_MAX)) {
		for (auto child : item.getChildren()) {
			child->setChecked(item.getChecked());
		}
	} else
		// Сообщаем об изменении видимости конкретного элемента неживой природы.
		globalWorld.setResVisibility(item.tag, item.getChecked());
}

void WindowsSettings::onTreeSpeciesCheckChanged(TreeItem &item)
{
	if (item.tag) {
		// Если указатель на вид организма, меняем видимость вида.
		demi::Species *spec = reinterpret_cast<demi::Species *>(item.tag);
		spec->setVisible(item.getChecked());
	}
	else {
		// Если указатель на генотип, надо поменять видимость у всех подчинённых узлов.
		for (auto child : item.getChildren()) {
			child->setChecked(item.getChecked());
		}
	}
}


// Сохраняет новое имя модели и обновляет надпись на экране.
void WindowsSettings::setModelFilename(const std::string &newName)
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
	// Обновим надписи с количествами, если панель не скрыта.
	if (!hidden())
		updateAmounts();
	
	// Автосохранение. Если прошёл час и установлена галочка регулярного автосохранения модели, сделаем это.
	//
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
	// Неживая природа.
	//
	// Удалим старые метки под количество элементов, если они были.
	// Сам список названий очистится при присвоении корневого узла.
	auto& children = panelElemAmounts->children();
	while (!children.empty())
		children.back()->remove_from_parent();

	// Корневой невидимый узел, дереву необходим для того, чтобы визуально могло быть сразу несколько узлов первого уровня.
	auto rootNodeE = std::make_shared<TreeItem>("", false, size_t(SIZE_MAX));	// В tag хранится индекс химэлемента, для корневого - заглушка.
	
	// Первый узел - под химические элементы.
	auto pSettings = globalWorld.getSettingsStorage();
	auto firstNodeE = std::make_shared<TreeItem>(pSettings->LocaleStr(cTreeInanimate), false, size_t(SIZE_MAX));

	// Добавляем сразу и метку в панель количества.
	panelElemAmounts->add_child(createLabelForAmount(pSettings->LocaleStr(cWindowsSettingsAmountsLabel)));

	// Добавляем химэлементы.
	for (size_t i = 0; i != globalWorld.getElemCount(); ++i) {
		// Чекбокс с названием элемента.
		firstNodeE->addChild(std::make_shared<TreeItem>(globalWorld.getResName(i), globalWorld.getResVisibility(i), i));

		// Метка под количество.
		panelElemAmounts->add_child(createLabelForAmount("-"));
	}

	rootNodeE->addChild(firstNodeE);

	// Добавляем дерево в список.
	pTreeViewElements->setRootItem(rootNodeE);
}

void WindowsSettings::initElemVisibilityTreeAfterRestart()
{
	// В отличие от обычной загрузки надо сохранить прежние значения видимости, то есть не у чекбоксов обновить 
	// поля на основе значений объектов, а обновить значения у объектов на основе чекбоксов.

	// Корневой узел.
	std::shared_ptr<TreeItem>& rootNodeE = pTreeViewElements->getRootItem();

	// Для неживой природы просто обновим значения.
	auto& inanimalNode = rootNodeE->getChildren().front();
	for (auto &child : inanimalNode->getChildren())
		globalWorld.setResVisibility(child->tag, child->getChecked());
}

// Обновляет отображение организмов (которые меняются в процессе расчёта).
void WindowsSettings::initAnimalVisibility()
{
	// Удалим старые надписи для количеств.
	auto& children = panelOrganismAmounts->children();
	while (!children.empty())
		children.back()->remove_from_parent();

	// Корневой невидимый узел, дереву необходим для того, чтобы визуально могло быть сразу несколько узлов первого уровня.
	auto rootNodeS = std::make_shared<TreeItem>("", false, size_t(SIZE_MAX));

	// Протоорганизм (вид).
	auto& tree = globalWorld.genotypesTree;
	auto& LUCAGenotype = tree.genotype;

	// Корневой узел под протовид.
	auto& LUCAGenotypeNode = std::make_shared<TreeItem>(cachedAnimalPrefixLabel + LUCAGenotype->getGenotypeName(), true, size_t(0));
	rootNodeS->addChild(LUCAGenotypeNode);

	// Надпись под количество.
	panelOrganismAmounts->add_child(createLabelForAmount("-"));

	// Добавляем виды организмов.  Для производных видов не доделано!
	for (auto& item : tree.species) {
		// Название генотипа указано ранее, выводим имена генов и их значения.
		auto curNode = std::make_shared<TreeItem>(item->getSpeciesName(), item->getVisible(), size_t(item.get()));
		LUCAGenotypeNode->addChild(curNode);
		panelOrganismAmounts->add_child(createLabelForAmount("-"));
	}

	pTreeViewSpecies->setRootItem(rootNodeS);
}


// Обновляет надписи с количествами.
void WindowsSettings::updateAmounts()
{
	// Неживая природа.
	//
	// Обновим надписи с количествами элементов согласно информации из специального объекта.
	// Пользуемся тем, что знаем порядок - первым идёт надпись напротив "неживая природа", далее первый элемент и т.д.
	// Так как set_text с использованием force_no_layout не проверяет, свёрнуто ли окно, сделаем это предварительно.
	const size_t cnt = globalWorld.getElemCount();
	auto childE = ++panelElemAmounts->children().begin();	// сразу пропускаем надпись-оглавление.
	for (size_t i = 0; i != cnt; ++i) {

		// Преобразуем указатель на базовый тип View на дочерний LabelView.
		std::shared_ptr<clan::LabelView> label = std::dynamic_pointer_cast<clan::LabelView>(*childE++);

		// Впишем количество с делением по разрядам.
		const std::string str = IntToStrWithDigitPlaces<unsigned long long>(globalWorld.amounts.getResAmounts(i));

		label->set_text(str, true);
	}


	// Живая природа.
	//

	// Если было обновление видов, переинициализируем надписи.
	auto& tree = globalWorld.genotypesTree;
	if (tree.flagSpaciesChanged) {

		// Сбрасываем флаг.
		tree.flagSpaciesChanged = false;

		// Инициалиируем надписи.
		initAnimalVisibility();
	}


	// Первая надпись - под корневой вид.
	//
	// Преобразуем указатель на базовый тип View на дочерний LabelView.
	auto& childIter = panelOrganismAmounts->children().begin();
	std::shared_ptr<clan::LabelView> label = std::dynamic_pointer_cast<clan::LabelView>(*childIter);
	// Впишем количество с делением по разрядам.
	label->set_text(IntToStrWithDigitPlaces<unsigned long long>(tree.genotype->getAliveCount()), true);

	// Остальные надписи под виды корневого. Для производных видов не доделано!
	for (auto& item : tree.species) {
		label = std::dynamic_pointer_cast<clan::LabelView>(*(++childIter));
		label->set_text(IntToStrWithDigitPlaces<unsigned long long>(item->getAliveCount()), true);
	}
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

// Сохраняет модель с показом диалогового окна.
void WindowsSettings::saveModel(const std::string& filename)
{
	// Создадим диалог с информационной надписью.
	auto pSettings = globalWorld.getSettingsStorage();
	const std::string caption(pSettings->LocaleStr(cWindowsSettingsDlgCaption));
	const std::string text(clan::string_format(pSettings->LocaleStr(cWindowsSettingsDlgSavingModel), filename));
	auto dialog = std::make_shared<MsgBox>(pSettings, text, caption, cMbNone);
	wManager->present_modal(this, dialog);
	dialog->initWindow(pSettings->fileResDoc.get_file_system());

	// Отрисуем вне очереди.
	dialog->immediate_update();

	try
	{
		globalWorld.saveModel(filename);
	}
	catch (...)
	{
		// Погасим диалог в случае ошибки.
		dialog->dismiss();
		throw;
	}

	// Погасим диалог.
	dialog->dismiss();
}

// Загружает модель с показом диалогового окна.
void WindowsSettings::loadModel(const std::string& filename)
{
	// Создадим диалог с информационной надписью.
	auto pSettings = globalWorld.getSettingsStorage();
	const std::string caption(pSettings->LocaleStr(cWindowsSettingsDlgCaption));
	const std::string text(clan::string_format(pSettings->LocaleStr(cWindowsSettingsDlgLoadingModel), filename));
	auto dialog = std::make_shared<MsgBox>(pSettings, text, caption, cMbNone);
	wManager->present_modal(this, dialog);
	dialog->initWindow(pSettings->fileResDoc.get_file_system());

	// Отрисуем вне очереди.
	dialog->immediate_update();

	try
	{
		globalWorld.loadModel(filename);
	}
	catch (...)
	{
		// Погасим диалог в случае ошибки.
		dialog->dismiss();
		throw;
	}

	// Погасим диалог.
	dialog->dismiss();
}


// trim from beginning of string (left)
inline std::string& ltrim(std::string& s)
{
	s.erase(0, s.find_first_not_of(' '));
	return s;
}