/* ===============================================================================
������������� �������� ������ ����.
���� � �����������.
27 may 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "settings_storage.h"

// ���� � ����������� ���������
auto cSettingsXML = "ThemeAero/settings.xml";

// �������������� �������� ����
auto cMainWindowSectionAppearance = "MainWindow/Appearance";
auto cMainWindowSectionLeft = "left";
auto cMainWindowSectionTop = "top";
auto cMainWindowSectionWidth = "width";
auto cMainWindowSectionHeight = "height";
auto cMainWindowSectionState = "state";
auto cMainWindowSectionFullScreen = "is_fullscreen";

// ��������� ������ ����
auto cMainWindowSectionMenu = "MainWindow/TopMenuState";
auto cMainWindowSectionSettingsWindowVisible = "SettingsWindowVisible";
auto cMainWindowSectionModelIlluminated = "ModelIlluminated";

// ���� ����������.
auto cMainWindowSectionLocale = "MainWindow/Locale";
auto cMainWindowSectionLanguage = "Language";
auto cLangSection = "Language";
auto cLangType = "lang";

// ���������� � �������
auto cProjectSectionName = "Project/ProjectName";
auto cProjectSectionNameFilename = "filename";

// �������� �������
auto cProjectSectionCheckboxes = "Project/Checkboxes";
auto cProjectSectionAutorun = "autorun";
auto cProjectSectionAutosave = "autosave";
auto cProjectSectionAutosaveHourly = "autosave_hourly";


SettingsStorage::SettingsStorage() : 
	sectionMainWindowApperance(XMLResDoc.get_resource(cMainWindowSectionAppearance).get_element()),
	sectionMainWindowTopMenu(XMLResDoc.get_resource(cMainWindowSectionMenu).get_element()),
	sectionProjectName(XMLResDoc.get_resource(cProjectSectionName).get_element()),
	sectionProjectCheckBoxes(XMLResDoc.get_resource(cProjectSectionCheckboxes).get_element()),
	resManager(clan::FileResourceManager::create(fileResDoc)),
	XMLResDoc(cSettingsXML), 
	fileResDoc(clan::FileSystem("ThemeAero"))
{
	// ���� ����������.
	auto sectionLocale = XMLResDoc.get_resource(cMainWindowSectionLocale).get_element();
	auto &lang = sectionLocale.get_attribute(cMainWindowSectionLanguage, "Eng");

	// �������� ��������� ������� � ������ �����.
	LoadLocaleStrings(lang);
}

SettingsStorage::~SettingsStorage()
{
	// ��������� ��������� �� �����.
	XMLResDoc.save(cSettingsXML);
}


// ���������� �������������� � ������� �������� ���� ���������.
const clan::Rectf SettingsStorage::getMainWindowPosition()
{
	// ��������� � ������� ����
	int mainWindowLeft = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionLeft, 0);
	int mainWindowTop = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionTop, 0);
	int mainWindowWidth = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionWidth, 800);
	int mainWindowHeight = sectionMainWindowApperance.get_attribute_int(cMainWindowSectionHeight, 600);

	return clan::Rectf(float(mainWindowLeft), float(mainWindowTop), float(mainWindowWidth), float(mainWindowHeight));
}


// ��������� �������� �������������� � ��������� �������� ���� ���������.
void SettingsStorage::setMainWindowSettings(const clan::Rectf & rect, 
	const clan::WindowShowType state, 
	bool isFullScreen)
{
	// ������� ������ ������ ���� ���� � ���������� ���������.
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
	// ��������� �������� ����.
	return clan::WindowShowType(sectionMainWindowApperance.get_attribute_int(cMainWindowSectionState, int(clan::WindowShowType::show_default)));
}

const bool SettingsStorage::getTopMenuIsModelIlluminated()
{
	// ���������� ��������� ������.
	return sectionMainWindowTopMenu.get_attribute_bool(cMainWindowSectionModelIlluminated, false);
}


void SettingsStorage::setTopMenuIsModelIlluminated(bool newValue)
{
	// ���������� ��������� ������.
	sectionMainWindowTopMenu.set_attribute_bool(cMainWindowSectionModelIlluminated, newValue);
}

const bool SettingsStorage::getTopMenuIsSettingsWindowVisible()
{
	// ����������� ������ � �����������.
	return sectionMainWindowTopMenu.get_attribute_bool(cMainWindowSectionSettingsWindowVisible, true);
}


void SettingsStorage::setTopMenuIsSettingsWindowVisible(bool newValue)
{
	// ����������� ������ � �����������.
	sectionMainWindowTopMenu.set_attribute_bool(cMainWindowSectionSettingsWindowVisible, newValue);
}

// ��� �������� ������� � ��������
const std::string SettingsStorage::getProjectFilename()
{
	// ��� �������� ������� � ��������
	return sectionProjectName.get_attribute(cProjectSectionNameFilename, "");
}

const bool SettingsStorage::getProjectAutorun()
{
	// ��� �������� ������� � ��������
	return sectionProjectCheckBoxes.get_attribute_bool(cProjectSectionAutorun, false);
}

const bool SettingsStorage::getProjectAutosave()
{
	// ��� �������� ������� � ��������
	return sectionProjectCheckBoxes.get_attribute_bool(cProjectSectionAutosave, false);
}

const bool SettingsStorage::getProjectAutosaveHourly()
{
	// ��� �������� ������� � ��������
	return sectionProjectCheckBoxes.get_attribute_bool(cProjectSectionAutosaveHourly, false);
}

void SettingsStorage::setProjectFilename(const std::string &newValue)
{
	// ��� �������� ������� � ��������
	sectionProjectName.set_attribute(cProjectSectionNameFilename, newValue);
}

void SettingsStorage::setProjectAutorun(bool newValue)
{
	// ��� �������� ������� � ��������
	sectionProjectCheckBoxes.set_attribute_bool(cProjectSectionAutorun, newValue);
}

void SettingsStorage::setProjectAutosave(bool newValue)
{
	// ��� �������� ������� � ��������
	sectionProjectCheckBoxes.set_attribute_bool(cProjectSectionAutosave, newValue);
}

void SettingsStorage::setProjectAutosaveHourly(bool newValue)
{
	// ��� �������� ������� � ��������
	sectionProjectCheckBoxes.set_attribute_bool(cProjectSectionAutosaveHourly, newValue);
}

std::string SettingsStorage::LocaleStr(const std::string &key)
{
	// ���������� ������ �� ��������� ����� ���� ��� ����, ���� �� �������.
	try {
		return langMap.at(key);
	}
	catch (...) {
		return key;
	}
}

// ��������� ������� �� �����.
void SettingsStorage::LoadLocaleStrings(const std::string &lang)
{
	// �������� ��������.
	const std::vector<std::string>& names = XMLResDoc.get_resource_names_of_type(cLangType, cLangSection);

	// ���������� ��������.
	int elemCount = int(names.size());

	// ��������� �������.
	for (int i = 0; i < elemCount; ++i) {

		// ��������� ������ � ������� �������.
		clan::XMLResourceNode &res = XMLResDoc.get_resource(names[i]);

		// �������� ��������.
		clan::DomElement &prop = res.get_element();

		// ����.
		auto &strKey = res.get_name();

		// �������� �� ������ �����.
		auto &strVal = prop.get_attribute(lang);

		// ��������� ���� ����-�������� ��� ������� �����.
		langMap[strKey] = strVal;
	}
}

// ����������� ������ �� UTF8 � CP1251 ��� ������������ OS Windows MessageBox.
std::string SettingsStorage::UTF8_to_CP1251(std::string const & utf8)
{
	if (!utf8.empty())
	{
		int wchlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
		if (wchlen > 0 && wchlen != 0xFFFD)
		{
			std::vector<wchar_t> wbuf(wchlen);
			MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], wchlen);
			std::vector<char> buf(wchlen);
			WideCharToMultiByte(1251, 0, &wbuf[0], wchlen, &buf[0], wchlen, 0, 0);

			return std::string(&buf[0], wchlen);
		}
	}
	return std::string();
}
