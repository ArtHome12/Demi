/* ===============================================================================
Моделирование эволюции живого мира.
Окно с настройками.
27 may 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

class SettingsStorage
{
public:
	SettingsStorage();
	~SettingsStorage();

	// Внешние ресурсы.
	clan::FileResourceDocument fileResDoc;
	clan::XMLResourceDocument XMLResDoc;
	clan::ResourceManager resManager;

	// Положение и состояние главного окна.
	const clan::Rectf getMainWindowPosition();
	const clan::WindowShowType getMainWindowState();
	const bool getIsFullScreen();
	void setMainWindowSettings(const clan::Rectf & rect,
		const clan::WindowShowType state, 
		bool isFullScreen);

	// Состояние панели меню.
	const bool getTopMenuIsSettingsWindowVisible();
	void setTopMenuIsSettingsWindowVisible(bool newValue);
	const bool getTopMenuIsModelIlluminated();
	void setTopMenuIsModelIlluminated(bool newValue);

	// Имя текущего проекта и чекбоксы
	const std::string getProjectFilename();
	const bool getProjectAutorun();
	const bool getProjectAutosave();
	const bool getProjectAutosaveHourly();
	void setProjectFilename(const std::string &newValue);
	void setProjectAutorun(bool newValue);
	void setProjectAutosave(bool newValue);
	void setProjectAutosaveHourly(bool newValue);

	// Возвращает строку по заданному ключу либо сам ключ, если не найдена.
	std::string LocaleStr(const std::string &key);

private:
	// Разделы XML-файла.
	clan::DomElement sectionMainWindowApperance;			// Раздел с параметрами главного окна.
	clan::DomElement sectionMainWindowTopMenu;				// Раздел с параметрами меню главного окна.
	clan::DomElement sectionProjectName;					// Раздел с названием файлов проекта.
	clan::DomElement sectionProjectCheckBoxes;				// Раздел с настройками проекта.

	// Локализованные строковые ресурсы.
	std::map <std::string, std::string> langMap;

	// Загружает локализованные строковые ресурсы из файла.
	void LoadLocaleStrings(const std::string &lang);
};

