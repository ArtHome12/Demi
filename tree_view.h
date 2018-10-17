/* ===============================================================================
������������� �������� ������ ����.
����������� ������ � ����������.
10 april 2017.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once


// ����������, ���� ������� ������ �� ������.
class ETreeItemNotFound : public std::exception
{
public:
	const std::string itemName;
	ETreeItemNotFound(const std::string& aItemName) : itemName(aItemName) {}
	virtual char const* what() const override
	{
		return std::string("Unknown item: " + itemName).c_str();
	}
};



class TreeView;

class TreeItem
{
public:
	TreeItem(const std::string& ItemCaption, bool itemChecked, size_t ATag) : caption(ItemCaption), tag(ATag), checked(itemChecked) {};

	bool collapsed = true;		// Collapsed or expanded the tree node.
	TreeItem* parent = nullptr;	// Parent node.

	// ���������� ��������� ������� � ������ children ����� ���������� ��� 0
	std::vector<std::shared_ptr<TreeItem>> getNextSibling(const std::vector<std::shared_ptr<TreeItem>> child);

	// �������� ��������.
	std::string caption;
	
	// �������������� ���� ��� ��������, ��������, �������.
	size_t tag;

	// ��� �������������� � ��������� ����������.
	std::weak_ptr<clan::CheckBoxView> view;

	// Callback ��� ������� �� ������������ �������.
	std::function<void(TreeItem&)> func_check_state_changed;

	// ������ � �����.
	bool getChecked() const { return checked; }
	void setChecked(bool aValue);
	void addChild(std::shared_ptr<TreeItem>& child);

	const std::vector<std::shared_ptr<TreeItem>>& getChildren() { return children; }

	// ���������� ������������ ���������, ���������� ������� ���������.
	void onStateChanged();

private:
	// ������� ��� ���.
	bool checked = false;

	// Children nodes.
	std::vector<std::shared_ptr<TreeItem>> children;
};


class TreeView : public clan::ScrollView
{
public:
	TreeView();
	~TreeView();

	// �������� �������� ��������� ������. �������� ������� �������, ����������� ���������� � ��� �����.
	void setRootItem(std::shared_ptr<TreeItem> item);

	// �������� �������� ������� ������.
	std::shared_ptr<TreeItem> getRootItem() { return rootItem; }

	/// ���������� ��������� ������� - ��������� � ������ �������.
	std::function<void(TreeItem &item)> func_check_changed;

	void layout_children(clan::Canvas &canvas) override;

private:
	// �������� ��������� �������, ��������� ��� ����, ����� ����� ���� ������� ��������� ������ ������� ������.
	std::shared_ptr<TreeItem> rootItem;

	// ��������� � ������ ����� ���������� �������� � ������� ��������.
	void addTreeNodeChildren(int margin, std::shared_ptr<TreeItem> item);

	// ���������� ������������ ���������.
	void onCheckChanged(TreeItem &item);
};