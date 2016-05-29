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

class WindowsSettings :	public clan::View
{
public:
	WindowsSettings(clan::Canvas &canvas);

private:
	// �������� ������� ������
	std::shared_ptr<clan::LabelView> lModelName;

	// ������� ����������� ������
	std::shared_ptr<clan::CheckBoxView> cbAutoRun;

	// ����������� �������
	void onButtondownOpen();
	void onButtondownSave();
	void onButtondownSaveAs();
	void onButtondownStartStop();
	void onButtondownRestart();
};

