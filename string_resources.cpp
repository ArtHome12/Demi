/* ===============================================================================
������������� �������� ������ ����.
������ ��� �������� �������������� ��������� ��������.
15 june 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "string_resources.h"

// ���������� ����������. ���������� ����������� � SettingsStorage.
StringResources globalStr;

// ����� � XML-�����.
const std::string cLangSection = "Language";
const std::string cLangType = "lang";


StringResources::StringResources()
{
}

const std::string &StringResources::getStr(const std::string &key)
{
	// ���������� ������ �� ��������� ����� ��� ����, ���� ����� ���.
	std::string res = langMap.at(key);
	return res == "" ? key : res;
}

// ��������� ������� �� �����.
void StringResources::LoadRes(const clan::XMLResourceDocument &resDoc, const std::string &lang)
{
	// �������� ��������.
	const std::vector<std::string>& names = resDoc.get_resource_names_of_type(cLangType, cLangSection);

	// ���������� ��������.
	int elemCount = int(names.size());

	// ��������� �������.
	for (int i = 0; i < elemCount; ++i) {

		// ��������� ������ � ������� �������.
		clan::XMLResourceNode &res = resDoc.get_resource(names[i]);

		// �������� ��������.
		clan::DomElement &prop = res.get_element();
		
		// ����.
		const std::string &strKey = res.get_name();

		// �������� �� ������ �����.
		const std::string &strVal = prop.get_attribute(lang);

		// ��������� ���� ����-�������� ��� ������� �����.
		langMap[strKey] = strVal;
	}
}
