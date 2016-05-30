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

#include <clanlib/xml.h>

class SettingsStorage
{
public:
	SettingsStorage();

	std::shared_ptr<clan::XMLResourceDocument> resDoc;

	// Положение и состояние главного окна.
	const clan::Rectf getMainWindowPosition();
	const bool getIsFullScreen() { return isFullScreen; }
	const clan::WindowShowType getMainWindowState() { return mainWindowState; }
	void setMainWindowPositionAndState(const clan::Rectf & rect, const clan::WindowShowType state, bool isFullScreen);

	// Имя текущего проекта и чекбоксы
	const std::string getProjectFilename() { return projectFilename; }
	const bool getProjectAutorun() { return projectAutorun; }
	const bool getProjectAutosave() { return projectAutosave; }
	const bool getProjectAutosaveHourly() { return projectAutosaveHourly; }
	void setProjectInfo(const std::string &projectFilename, bool autorun, bool autosave, bool autosaveHourly);


private:
	// Положение и размеры окна
	int mainWindowLeft = 0;
	int mainWindowTop = 0;
	int mainWindowWidth = 800;
	int mainWindowHeight = 600;
	clan::WindowShowType mainWindowState = clan::WindowShowType::show_default;
	bool isFullScreen = false;

	// Имя текущего проекта и чекбоксы
	std::string projectFilename = "";
	bool projectAutorun = false;
	bool projectAutosave = false;
	bool projectAutosaveHourly = false;
};

