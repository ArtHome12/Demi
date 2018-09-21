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
#include "tree_view.h"


class WindowsSettings :	public clan::View
{
public:
	WindowsSettings(clan::Canvas &canvas, std::shared_ptr<SettingsStorage> pSettingsStorage);
	~WindowsSettings();

	// ��� ��������� ����������� �� ��������� ��������� ����� (��� ���������� �������������� ������, ���������� ���������).
	void modelRenderNotify(size_t secondsElapsed);

private:
	// �������� ������� ������
	std::string modelFilename;
	std::shared_ptr<clan::LabelView> pLabelModelName;

	// ������ �������� ����� ������.
	std::shared_ptr<clan::ButtonView> pButtonNew;

	// ������ �������� ������.
	std::shared_ptr<clan::ButtonView> pButtonOpen;

	// ������ ����������� ������.
	std::shared_ptr<clan::ButtonView> pButtonRestart;

	// ������ ������� ��� ������������ �������.
	std::shared_ptr<clan::ButtonView> pButtonRunPause;

	// ������� ����������� ������
	std::shared_ptr<clan::CheckBoxView> pCBAutoRun;

	// ������� �������������� ������ ��� ������ �� ���������.
	std::shared_ptr<clan::CheckBoxView> pCBAutoSave;

	// ������� �������������� (����������) �������������� ������.
	std::shared_ptr<clan::CheckBoxView> pCBAutoSaveHourly;

	// ������ � ��������� ��������� ���������.
	std::shared_ptr<TreeView> pTreeView;

	// ������ ��� ���������� ���������.
	std::shared_ptr<View> panelElemAmounts;

	// ���������.
	std::shared_ptr<SettingsStorage> pSettings;

	// ���������� ��������� ������ � ������� ������ ���������.
	size_t secondsElapsed = 0;

	// ����������� �������
	void onButtondownNew();
	void onButtondownOpen();
	void onButtondownSave();
	void onButtondownSaveAs();
	void onButtondownRunPause();
	void onButtondownRestart();
	void onCBAutoRunToggle();
	void onCBAutoSaveToggle();
	void onCBAutoSaveHourlyToggle();

	// ���������� �������, ���������� ����� ������������ ������� �� �������� ������������ ������.
	void onTreeCheckChanged(TreeItem &item);

	// ��������� ����� ��� ������ � ��������� ������� �� ������.
	void set_modelFilename(const std::string &newName);

	// ��������� ������ � ��������� ��������� ��������� �� ��������� �������� � ������.
	void initElemVisibilityTree();

	// ���� ����������� �� ��������� � ������� ���������.
	void initElemVisibilityTreeAfterRestart();

	// ������ �������.
	std::shared_ptr<clan::LabelView> createLabelForAmount(std::string text);
};

