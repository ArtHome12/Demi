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

namespace demi {
	class GenotypesTree;
}
class TreeItem;
class TreeView;
class GenotypesTreeHelperItem;

class WindowsSettings :	public clan::View
{
public:
	WindowsSettings();
	~WindowsSettings();

	// ����������� ����� ���������, ������� ��������� ����.
	void initWindow(clan::Canvas& canvas);

	// ��� �������������� �������� ������.
	void finishInit(clan::WindowManager* windowManager);

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

	// ������ ���������� ������.
	std::shared_ptr<clan::ButtonView> pButtonSave;

	// ������ ���������� ������ ��� ����� ������.
	std::shared_ptr<clan::ButtonView> pButtonSaveAs;

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
	std::shared_ptr<TreeView> pTreeViewElements;
	std::shared_ptr<TreeView> pTreeViewSpecies;

	// ������ ��� ���������� ���������.
	std::shared_ptr<View> panelElemAmounts;
	std::shared_ptr<View> panelOrganismAmounts;

	// ���������� ��������� ������ � ������� ������ ���������.
	size_t secondsElapsed = 0;

	// ������������ ������� ��� ������� ��� ����� ����������, ��� ���������.
	std::string cachedAnimalPrefixLabel;

	// ��� �������� � �������������� �� ���������� ������ ���������� ��������� �������� 

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
	void onTreeElementsCheckChanged(TreeItem &item);
	void onTreeSpeciesCheckChanged(TreeItem &item);

	// ��������� ����� ��� ������ � ��������� ������� �� ������.
	void setModelFilename(const std::string &newName);

	// ��������� ������ � ��������� ��������� ��������� �� ��������� �������� � ������.
	void initElemVisibilityTree();

	// � ������� �� ������� �������� ���� ��������� ������� �������� ���������.
	void initElemVisibilityTreeAfterRestart();

	// ��������� ����������� ���������� (������� �������� � �������� �������).
	void initAnimalVisibility();
	void doInitAnimalVisibility(std::shared_ptr<demi::GenotypesTree> treeNode, std::shared_ptr<TreeItem> item);

	// ������ �������.
	std::shared_ptr<clan::LabelView> createLabelForAmount(std::string text);

	clan::WindowManager* wManager;

	// �������������� ������ ��������� � ������ ��� ������������� ��� ���������� ���������.
	std::vector<GenotypesTreeHelperItem*> treeBackup;

	// ��������� ������ � ������� ����������� ����.
	void saveModel(const std::string& filename);

	// ��������� ������ � ������� ����������� ����.
	void loadModel(const std::string& filename);

	// ��������� ������� � ������������.
	void updateAmounts();

	// ���������� ��������� �� ������.
	void showError(const std::string& errMessage);
};

