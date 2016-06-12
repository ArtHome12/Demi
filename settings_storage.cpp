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
#include "settings_storage.h"

// Файл с настройками программы
const std::string cSettingsXML = "ThemeAero/settings.xml";

// Местоположение главного окна
const std::string cMainWindowSectionAppearance = "MainWindow/Appearance";
const std::string cMainWindowSectionLeft = "left";
const std::string cMainWindowSectionTop = "top";
const std::string cMainWindowSectionWidth = "width";
const std::string cMainWindowSectionHeight = "height";
const std::string cMainWindowSectionState = "state";
const std::string cMainWindowSectionFullScreen = "is_fullscreen";

// Состояние панели меню
const std::string cMainWindowSectionMenu = "MainWindow/TopMenuState";
const std::string cMainWindowSectionSettingsWindowVisible = "SettingsWindowVisible";
const std::string cMainWindowSectionModelIlluminated = "ModelIlluminated";

// Информация о проекте
const std::string cProjectSectionName = "Project/ProjectName";
const std::string cProjectSectionNameFilename = "filename";

// Чекбоксы проекта
const std::string cProjectSectionCheckboxes = "Project/Checkboxes";
const std::string cProjectSectionAutorun = "autorun";
const std::string cProjectSectionAutosave = "autosave";
const std::string cProjectSectionAutosaveHourly = "autosave_hourly";

SettingsStorage::SettingsStorage() : resManager(clan::FileResourceManager::create(fileResDoc)),
	XMLResDoc(cSettingsXML), 
	fileResDoc(clan::FileSystem("ThemeAero"))
{
	// Читаем раздел с размерами.
	clan::DomElement &prop = XMLResDoc.get_resource(cMainWindowSectionAppearance).get_element();

	mainWindowLeft = prop.get_attribute_int(cMainWindowSectionLeft, mainWindowLeft);
	mainWindowTop = prop.get_attribute_int(cMainWindowSectionTop, mainWindowTop);
	mainWindowWidth = prop.get_attribute_int(cMainWindowSectionWidth, mainWindowWidth);
	mainWindowHeight = prop.get_attribute_int(cMainWindowSectionHeight, mainWindowHeight);
	mainWindowState = clan::WindowShowType(prop.get_attribute_int(cMainWindowSectionState, int(mainWindowState)));
	isFullScreen = prop.get_attribute_bool(cMainWindowSectionFullScreen, isFullScreen);

	// Состояние панели меню
	prop = XMLResDoc.get_resource(cMainWindowSectionMenu).get_element();
	topMenuSettingsWindowVisible = prop.get_attribute_bool(cMainWindowSectionSettingsWindowVisible, topMenuSettingsWindowVisible);
	topMenuModelIlluminated = prop.get_attribute_bool(cMainWindowSectionModelIlluminated, topMenuModelIlluminated);

	// Имя текущего проекта и чекбоксы
	prop = XMLResDoc.get_resource(cProjectSectionName).get_element();
	projectFilename = prop.get_attribute(cProjectSectionNameFilename, "");
	prop = XMLResDoc.get_resource(cProjectSectionCheckboxes).get_element();
	projectAutorun = prop.get_attribute_bool(cProjectSectionAutorun, projectAutorun);
	projectAutosave = prop.get_attribute_bool(cProjectSectionAutosave, projectAutosave);
	projectAutosaveHourly = prop.get_attribute_bool(cProjectSectionAutosaveHourly, projectAutosaveHourly);
}


// Возвращает местоположение и размеры главного окна программы.
const clan::Rectf SettingsStorage::getMainWindowPosition()
{
	return clan::Rect(mainWindowLeft, mainWindowTop, mainWindowWidth, mainWindowHeight);
}


// Обновляет значения местоположения и состояния главного окна программы.
void SettingsStorage::setMainWindowSettings(const clan::Rectf & rect, 
	const clan::WindowShowType state, 
	bool isFullScreen,
	bool isWindowsSettingsVisible,
	bool isModelIlluminated)
{
	// Раздел с параметрами главного окна.
	clan::DomElement &prop = XMLResDoc.get_resource(cMainWindowSectionAppearance).get_element();

	// Признак наличия изменений.
	bool dirt = false;

	// Размеры меняем только если окно в нормальном состоянии.
	if ((state != clan::WindowShowType::maximize) && (state != clan::WindowShowType::minimize) && !isFullScreen) {
		if (mainWindowLeft != int(rect.left)) {
			mainWindowLeft = int(rect.left);
			prop.set_attribute_int(cMainWindowSectionLeft, mainWindowLeft);
			dirt = true;
		}

		if (mainWindowTop != int(rect.top)) {
			mainWindowTop = int(rect.top);
			prop.set_attribute_int(cMainWindowSectionTop, mainWindowTop);
			dirt = true;
		}

		if (mainWindowWidth != int(rect.right)) {
			mainWindowWidth = int(rect.right);
			prop.set_attribute_int(cMainWindowSectionWidth, mainWindowWidth);
			dirt = true;
		}

		if (mainWindowHeight != int(rect.bottom)) {
			mainWindowHeight = int(rect.bottom);
			prop.set_attribute_int(cMainWindowSectionHeight, mainWindowHeight);
			dirt = true;
		}
	}

	if (mainWindowState != state) {
		mainWindowState = state;
		prop.set_attribute_int(cMainWindowSectionState, int(mainWindowState));
		dirt = true;
	}

	if (this->isFullScreen != isFullScreen) {
		this->isFullScreen = isFullScreen;
		prop.set_attribute_bool(cMainWindowSectionFullScreen, this->isFullScreen);
		dirt = true;
	}

	// Раздел с параметрами меню главного окна.
	prop = XMLResDoc.get_resource(cMainWindowSectionMenu).get_element();
	if (topMenuSettingsWindowVisible != isWindowsSettingsVisible) {
		topMenuSettingsWindowVisible = isWindowsSettingsVisible;
		prop.set_attribute_bool(cMainWindowSectionSettingsWindowVisible, topMenuSettingsWindowVisible);
		dirt = true;
	}

	if (topMenuModelIlluminated != isModelIlluminated) {
		topMenuModelIlluminated = isModelIlluminated;
		prop.set_attribute_bool(cMainWindowSectionModelIlluminated, topMenuModelIlluminated);
		dirt = true;
	}

	// Сохраняем изменения на диске.
	if (dirt)
		XMLResDoc.save(cSettingsXML);
}


void SettingsStorage::setProjectInfo(const std::string &projectFilename, bool autorun, bool autosave, bool autosaveHourly)
{
	// Раздел с параметрами проекта.
	clan::DomElement &prop = XMLResDoc.get_resource(cProjectSectionName).get_element();

	// Признак наличия изменений.
	bool dirt = false;

	if (this->projectFilename != projectFilename) {
		this->projectFilename = projectFilename;
		prop.set_attribute(cProjectSectionNameFilename, this->projectFilename);
		dirt = true;
	}

	// Раздел с чекбоксами проекта.
	clan::DomElement &propCB = XMLResDoc.get_resource(cProjectSectionCheckboxes).get_element();

	if (projectAutorun != autorun) {
		projectAutorun = autorun;
		propCB.set_attribute_bool(cProjectSectionAutorun, projectAutorun);
		dirt = true;
	}

	if (projectAutosave != autosave) {
		projectAutosave = autosave;
		propCB.set_attribute_bool(cProjectSectionAutosave, projectAutosave);
		dirt = true;
	}

	if (projectAutosaveHourly != autosaveHourly) {
		projectAutosaveHourly = autosaveHourly;
		propCB.set_attribute_bool(cProjectSectionAutosaveHourly, projectAutosaveHourly);
		dirt = true;
	}

	// Сохраняем изменения на диске.
	if (dirt)
		XMLResDoc.save(cSettingsXML);
}
