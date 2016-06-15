/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для хранения локализованных текстовых ресурсов.
15 june 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once
class StringResources
{
public:
	StringResources();

	// Возвращает строку по заданному ключу.
	const std::string &getStr(const std::string &key);

	// Загружает ресурсы из файла.
	void LoadRes(const clan::XMLResourceDocument &resDoc, const std::string &lang);

private:
	std::map <std::string, std::string> langMap;
};

// Глобальная переменная.
extern StringResources globalStr;
