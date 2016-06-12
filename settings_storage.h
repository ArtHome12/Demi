/* ===============================================================================
������������� �������� ������ ����.
���� � �����������.
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

	// ������� �������.
	clan::FileResourceDocument fileResDoc;
	clan::XMLResourceDocument XMLResDoc;
	clan::ResourceManager resManager;

	// ��������� � ��������� �������� ����.
	const clan::Rectf getMainWindowPosition();
	const bool getIsFullScreen() { return isFullScreen; }
	const clan::WindowShowType getMainWindowState() { return mainWindowState; }
	void setMainWindowSettings(const clan::Rectf & rect, 
		const clan::WindowShowType state, 
		bool isFullScreen, 
		bool isWindowsSettingsVisible, 
		bool isModelIlluminated);

	// ��������� ������ ����.
	const bool getTopMenuIsSettingsWindowVisible() { return topMenuSettingsWindowVisible; }
	const bool getTopMenuIsModelIlluminated() { return topMenuModelIlluminated; }

	// ��� �������� ������� � ��������
	const std::string getProjectFilename() { return projectFilename; }
	const bool getProjectAutorun() { return projectAutorun; }
	const bool getProjectAutosave() { return projectAutosave; }
	const bool getProjectAutosaveHourly() { return projectAutosaveHourly; }
	void setProjectInfo(const std::string &projectFilename, bool autorun, bool autosave, bool autosaveHourly);


private:
	// ��������� � ������� ����
	int mainWindowLeft = 0;
	int mainWindowTop = 0;
	int mainWindowWidth = 800;
	int mainWindowHeight = 600;
	clan::WindowShowType mainWindowState = clan::WindowShowType::show_default;
	bool isFullScreen = false;

	// ��������� ������ ����
	bool topMenuSettingsWindowVisible = false;
	bool topMenuModelIlluminated = false;

	// ��� �������� ������� � ��������
	std::string projectFilename = "";
	bool projectAutorun = false;
	bool projectAutosave = false;
	bool projectAutosaveHourly = false;
};

