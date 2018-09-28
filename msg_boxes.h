/* ===============================================================================
������������� �������� ������ ����.
�������������������� �������.
26 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

#include "settings_storage.h"

enum eMbType { cMbOkCancel };
enum eMbResultType { cMbResultOk, cMbResultCancel };


class MsgBox : public clan::WindowController
{
public:
	MsgBox(SettingsStorage* pSettings, const std::string& text, const std::string& caption, eMbType mbType);
	~MsgBox();

	// ��� �������� ������, ��� ������ ������� � ������� ����� ������ �����.
	void loadIcons(clan::Canvas &canvas, const clan::FileSystem& fs);

	// ���������� �������� ����������� ����.
	void on_window_close();

	// Callback ��� ������� �� ��������.
	std::function<void(eMbResultType)> onProcessResult;

private:
	// ������� ���������, ����� ���� ������ ������������� ������.
	eMbResultType result = cMbResultCancel;

	std::shared_ptr<clan::ImageView> leftIcon;
	std::shared_ptr<clan::ButtonView> bOk;
	std::shared_ptr<clan::ButtonView> bCancel;
};