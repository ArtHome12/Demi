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
	~SettingsStorage();

	// ������� �������.
	clan::FileResourceDocument fileResDoc;
	clan::XMLResourceDocument XMLResDoc;
	clan::ResourceManager resManager;

	// ��������� � ��������� �������� ����.
	const clan::Rectf getMainWindowPosition();
	const clan::WindowShowType getMainWindowState();
	const bool getIsFullScreen();
	void setMainWindowSettings(const clan::Rectf & rect,
		const clan::WindowShowType state, 
		bool isFullScreen);

	// ��������� ������ ����.
	const bool getTopMenuIsSettingsWindowVisible();
	void setTopMenuIsSettingsWindowVisible(bool newValue);
	const bool getTopMenuIsModelIlluminated();
	void setTopMenuIsModelIlluminated(bool newValue);

	// ��� �������� ������� � ��������
	const std::string getProjectFilename();
	const bool getProjectAutorun();
	const bool getProjectAutosave();
	const bool getProjectAutosaveHourly();
	void setProjectFilename(const std::string &newValue);
	void setProjectAutorun(bool newValue);
	void setProjectAutosave(bool newValue);
	void setProjectAutosaveHourly(bool newValue);

	// ���������� ������ �� ��������� ����� ���� ��� ����, ���� �� �������.
	std::string LocaleStr(const std::string &key);

private:
	// ������� XML-�����.
	clan::DomElement sectionMainWindowApperance;			// ������ � ����������� �������� ����.
	clan::DomElement sectionMainWindowTopMenu;				// ������ � ����������� ���� �������� ����.
	clan::DomElement sectionProjectName;					// ������ � ��������� ������ �������.
	clan::DomElement sectionProjectCheckBoxes;				// ������ � ����������� �������.

	// �������������� ��������� �������.
	std::map <std::string, std::string> langMap;

	// ��������� �������������� ��������� ������� �� �����.
	void LoadLocaleStrings(const std::string &lang);
};

