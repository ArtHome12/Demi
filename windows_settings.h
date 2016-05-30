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

#include "settings_storage.h"


class WindowsSettings :	public clan::View
{
public:
	WindowsSettings(clan::Canvas &canvas);
	~WindowsSettings();

private:

	// Настройки
	std::shared_ptr<SettingsStorage> settings;

	// Название текущей модели
	std::string modelFilename;
	std::shared_ptr<clan::LabelView> lModelName;

	// Чекбокс автозапуска модели
	std::shared_ptr<clan::CheckBoxView> cbAutoRun;

	// Чекбокс автосохранения модели при выходе из программы.
	std::shared_ptr<clan::CheckBoxView> cbAutoSave;

	// Чекбокс периодического (ежечасного) автосохранения модели.
	std::shared_ptr<clan::CheckBoxView> cbAutoSaveHourly;

	// Обработчики событий
	void onButtondownNew();
	void onButtondownOpen();
	void onButtondownSave();
	void onButtondownSaveAs();
	void onButtondownStartStop();
	void onButtondownRestart();

	// Сохраняет новое имя модели и обновляет надпись на экране.
	void set_modelFilename(const std::string &newName);
};

