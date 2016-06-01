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

#include "settings_storage.h"


class WindowsSettings :	public clan::View
{
public:
	WindowsSettings(clan::Canvas &canvas);
	~WindowsSettings();

private:

	// ���������
	std::shared_ptr<SettingsStorage> pSettings;

	// �������� ������� ������
	std::string modelFilename;
	std::shared_ptr<clan::LabelView> pLabelModelName;

	// ������ ������� ��� ������������ �������.
	std::shared_ptr<clan::ButtonView> pButtonRunPause;

	// ������� ����������� ������
	std::shared_ptr<clan::CheckBoxView> pCBAutoRun;

	// ������� �������������� ������ ��� ������ �� ���������.
	std::shared_ptr<clan::CheckBoxView> pCBAutoSave;

	// ������� �������������� (����������) �������������� ������.
	std::shared_ptr<clan::CheckBoxView> pCBAutoSaveHourly;

	// ����������� �������
	void onButtondownNew();
	void onButtondownOpen();
	void onButtondownSave();
	void onButtondownSaveAs();
	void onButtondownRunPause();
	void onButtondownRestart();

	// ��������� ����� ��� ������ � ��������� ������� �� ������.
	void set_modelFilename(const std::string &newName);
};

