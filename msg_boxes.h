/* ===============================================================================
Моделирование эволюции живого мира.
Платформонезависимые диалоги.
26 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

#include "settings_storage.h"


class MsgBox : public clan::WindowController
{
public:
	enum eMbType { cMbNone, cMbOkCancel, cMbOk, cMbCancel };
	enum eMbResultType { cMbResultOk, cMbResultCancel };

	MsgBox(SettingsStorage* pSettings, const std::string& text, const std::string& caption, eMbType mbType);
	~MsgBox();

	// Должна быть вызвана после создания оконной структуры.
	void initWindow(const clan::FileSystem& fs);

	// Callback для события на закрытие.
	std::function<void(eMbResultType)> onProcessResult;

private:
	// Текущий результат, может быть изменён обработчиками кнопок.
	eMbResultType result = cMbResultCancel;

	std::shared_ptr<clan::ImageView> leftIcon;
	std::shared_ptr<clan::ButtonView> bOk;
	std::shared_ptr<clan::ButtonView> bCancel;

	void on_input_down(const clan::KeyEvent &e);
};