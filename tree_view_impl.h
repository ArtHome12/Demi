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

class TreeView_Impl
{
public:
	//void on_key_press(clan::KeyEvent &e);
	//void on_pointer_press(clan::PointerEvent &e);
	//void on_pointer_release(clan::PointerEvent &e);
	//void on_pointer_enter(clan::PointerEvent &e);
	//void on_pointer_leave(clan::PointerEvent &e);

	//void set_hot_item(int index);

	TreeView *treeView = nullptr;

	std::shared_ptr<TreeItem> rootItem;

	/// Обработчик изменения отметки - установки и снятия галочки.
	std::function<void(TreeItem &item)> func_check_changed;

	// Добавляет к списку детей указанного элемента с большим отступом.
	void addTreeNodeChildren(int margin, std::shared_ptr<TreeItem> item);

	// Обработчик переключения видимости.
	void onCheckChanged(TreeItem &item);

private:
	//int get_selection_index(clan::PointerEvent &e);

};