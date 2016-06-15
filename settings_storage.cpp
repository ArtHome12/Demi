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
#include "string_resources.h"
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

// Язык интерфейса.
const std::string cMainWindowSectionLocale = "MainWindow/Locale";
const std::string cMainWindowSectionLanguage = "Language";

// Информация о проекте
const std::string cProjectSectionName = "Project/ProjectName";
const std::string cProjectSectionNameFilename = "filename";

// Чекбоксы проекта
const std::string cProjectSectionCheckboxes = "Project/Checkboxes";
const std::string cProjectSectionAutorun = "autorun";
const std::string cProjectSectionAutosave = "autosave";
const std::string cProjectSectionAutosaveHourly = "autosave_hourly";

SettingsStorage::SettingsStorage() : 
	sectionMainWindowApperance(XMLResDoc.get_resource(cMainWindowSectionAppearance).get_element()),
	sectionMainWindowTopMenu(XMLResDoc.get_resource(cMainWindowSectionMenu).get_element()),
	sectionProjectName(XMLResDoc.get_resource(cProjectSectionName).get_element()),
	sectionProjectCheckBoxes(XMLResDoc.get_resource(cProjectSectionCheckboxes).get_element()),
	resManager(clan::FileResourceManager::create(fileResDoc)),
	XMLResDoc(cSettingsXML), 
	fileResDoc(clan::FileSystem("ThemeAero"))
{
	// Язык интерфейса.
	auto sectionLocale = XMLResDoc.get_resource(cMainWindowSectionLocale).get_element();
	const std::string &lang = sectionLocale.get_attribute(cMainWindowSectionLanguage, "Eng");

	// Загрузим строковые ресурсы с учётом языка.
	globalStr.LoadRes(XMLResDoc, lang);
}

SettingsStorage::~SettingsStorage()
{
	// Сохраняем изменения на диске.
	XMLResDoc.save(cSettingsXML);
}


// Возвращает местоположение и размеры главного окна программы.
const clan::Rectf SettingsStorage::getMainWindowPosition()
{
	// Положение и размеры окна
	int mainWindowLeft = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionLeft, 0);
	int mainWindowTop = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionTop, 0);
	int mainWindowWidth = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionWidth, 800);
	int mainWindowHeight = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionHeight, 600);

	return clan::Rectf(float(mainWindowLeft), float(mainWindowTop), float(mainWindowWidth), float(mainWindowHeight));
}


// Обновляет значения местоположения и состояния главного окна программы.
void SettingsStorage::setMainWindowSettings(const clan::Rectf & rect, 
	const clan::WindowShowType state, 
	bool isFullScreen)
{
	// Размеры меняем только если окно в нормальном состоянии.
	if ((state != clan::WindowShowType::maximize) && (state != clan::WindowShowType::minimize) && !isFullScreen) {
		sectionMainWindowApperance.set_attribute_int(cMainWindowSectionLeft, int(rect.left));
		sectionMainWindowApperance.set_attribute_int(cMainWindowSectionTop, int(rect.top));
		sectionMainWindowApperance.set_attribute_int(cMainWindowSectionWidth, int(rect.right));
		sectionMainWindowApperance.set_attribute_int(cMainWindowSectionHeight, int(rect.bottom));
	}

	sectionMainWindowApperance.set_attribute_int(cMainWindowSectionState, int(state));
	sectionMainWindowApperance.set_attribute_bool(cMainWindowSectionFullScreen, isFullScreen);
}

const bool SettingsStorage::getIsFullScreen()
{
	return sectionMainWindowApperance.get_attribute_bool(cMainWindowSectionFullScreen, false);
}

const clan::WindowShowType SettingsStorage::getMainWindowState()
{
	// Состояние гравного окна.
	return clan::WindowShowType(sectionMainWindowApperance.get_attribute_int(cMainWindowSectionState, int(clan::WindowShowType::show_default)));
}

const bool SettingsStorage::getTopMenuIsModelIlluminated()
{
	// Постоянная подсветка модели.
	return sectionMainWindowTopMenu.get_attribute_bool(cMainWindowSectionModelIlluminated, false);
}


void SettingsStorage::setTopMenuIsModelIlluminated(bool newValue)
{
	// Постоянная подсветка модели.
	sectionMainWindowTopMenu.set_attribute_bool(cMainWindowSectionModelIlluminated, newValue);
}

const bool SettingsStorage::getTopMenuIsSettingsWindowVisible()
{
	// Отображение панели с настройками.
	return sectionMainWindowTopMenu.get_attribute_bool(cMainWindowSectionSettingsWindowVisible, true);
}


void SettingsStorage::setTopMenuIsSettingsWindowVisible(bool newValue)
{
	// Отображение панели с настройками.
	sectionMainWindowTopMenu.set_attribute_bool(cMainWindowSectionSettingsWindowVisible, newValue);
}

// Имя текущего проекта и чекбоксы
const std::string SettingsStorage::getProjectFilename()
{
	// Имя текущего проекта и чекбоксы
	return sectionProjectName.get_attribute(cProjectSectionNameFilename, "");
}

const bool SettingsStorage::getProjectAutorun()
{
	// Имя текущего проекта и чекбоксы
	return sectionProjectCheckBoxes.get_attribute_bool(cProjectSectionAutorun, false);
}

const bool SettingsStorage::getProjectAutosave()
{
	// Имя текущего проекта и чекбоксы
	return sectionProjectCheckBoxes.get_attribute_bool(cProjectSectionAutosave, false);
}

const bool SettingsStorage::getProjectAutosaveHourly()
{
	// Имя текущего проекта и чекбоксы
	return sectionProjectCheckBoxes.get_attribute_bool(cProjectSectionAutosaveHourly, false);
}

void SettingsStorage::setProjectFilename(const std::string &newValue)
{
	// Имя текущего проекта и чекбоксы
	sectionProjectName.set_attribute(cProjectSectionNameFilename, newValue);
}

void SettingsStorage::setProjectAutorun(bool newValue)
{
	// Имя текущего проекта и чекбоксы
	sectionProjectCheckBoxes.set_attribute_bool(cProjectSectionAutorun, newValue);
}

void SettingsStorage::setProjectAutosave(bool newValue)
{
	// Имя текущего проекта и чекбоксы
	sectionProjectCheckBoxes.set_attribute_bool(cProjectSectionAutosave, newValue);
}

void SettingsStorage::setProjectAutosaveHourly(bool newValue)
{
	// Имя текущего проекта и чекбоксы
	sectionProjectCheckBoxes.set_attribute_bool(cProjectSectionAutosaveHourly, newValue);
}
