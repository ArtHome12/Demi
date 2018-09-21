/* ===============================================================================
Моделирование эволюции живого мира.
Древовидный список с чекбоксами.
10 april 2017.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once


class TreeView_Impl;


class TreeItem
{
public:
	//TreeItem() {};
	TreeItem(const std::string& ItemCaption, int ATag, bool itemChecked = false) : caption(ItemCaption), tag(ATag), checked(itemChecked) {};

	bool collapsed = true;								// Collapsed or expanded the tree node.
	std::weak_ptr<TreeItem> wpParent;					// Parent node.
	std::vector<std::shared_ptr<TreeItem>> children;	// Children node.

	// Возвращает следующий элемент в списке children после указанного или 0
	std::vector<std::shared_ptr<TreeItem>> getNextSibling(const std::vector<std::shared_ptr<TreeItem>> child);

	// Название элемента.
	std::string caption;
	
	// Отмечен или нет.
	bool checked = false;

	// Дополнительное поле для хранения, например, индекса.
	int tag;

	// Обработчик переключения видимости.
	void onStateChanged();

	// Callback для события на переключение галочки.
	std::function<void(TreeItem&)> func_check_state_changed;

};


class TreeView : public clan::ScrollView
{
public:
	TreeView();
	~TreeView();

	/// Передать иерархию элементов дереву. Корневой элемент невидим, отображение начинается с его детей.
	void set_root_item(std::shared_ptr<TreeItem> item);

	/// Получить корневой элемент дерева.
	std::shared_ptr<TreeItem> get_root_item();

	/// Обработчик изменения отметки - установки и снятия галочки.
	std::function<void(TreeItem &item)> &func_check_changed();

	void layout_children(clan::Canvas &canvas) override;

private:
	std::unique_ptr<TreeView_Impl> impl;
};