/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для хранения локализованных текстовых ресурсов.
15 june 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "string_resources.h"

// Глобальная переменная. Фактически загружается в SettingsStorage.
StringResources globalStr;

// Ключи в XML-файле.
const std::string cLangSection = "Language";
const std::string cLangType = "lang";


StringResources::StringResources()
{
}

const std::string &StringResources::getStr(const std::string &key)
{
	// Возвращает строку по заданному ключу или ключ, если такой нет.
	std::string res = langMap.at(key);
	return res == "" ? key : res;
}

// Загружает ресурсы из файла.
void StringResources::LoadRes(const clan::XMLResourceDocument &resDoc, const std::string &lang)
{
	// Названия ресурсов.
	const std::vector<std::string>& names = resDoc.get_resource_names_of_type(cLangType, cLangSection);

	// Количество ресурсов.
	int elemCount = int(names.size());

	// Считываем ресурсы.
	for (int i = 0; i < elemCount; ++i) {

		// Строковый ресурс с разными языками.
		clan::XMLResourceNode &res = resDoc.get_resource(names[i]);

		// Свойства элемента.
		clan::DomElement &prop = res.get_element();
		
		// Ключ.
		const std::string &strKey = res.get_name();

		// Значение на нужном языке.
		const std::string &strVal = prop.get_attribute(lang);

		// Сохраняем пару ключ-значение для нужного языка.
		langMap[strKey] = strVal;
	}
}
