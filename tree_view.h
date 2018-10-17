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


// Исключение, если элемент дерева не найден.
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

	// Возвращает следующий элемент в списке children после указанного или 0
	std::vector<std::shared_ptr<TreeItem>> getNextSibling(const std::vector<std::shared_ptr<TreeItem>> child);

	// Название элемента.
	std::string caption;
	
	// Дополнительное поле для хранения, например, индекса.
	size_t tag;

	// Для взаимодействия с элементом интерфейса.
	std::weak_ptr<clan::CheckBoxView> view;

	// Callback для события на переключение галочки.
	std::function<void(TreeItem&)> func_check_state_changed;

	// Доступ к полям.
	bool getChecked() const { return checked; }
	void setChecked(bool aValue);
	void addChild(std::shared_ptr<TreeItem>& child);

	const std::vector<std::shared_ptr<TreeItem>>& getChildren() { return children; }

	// Обработчик переключения видимости, вызывается эконным элементом.
	void onStateChanged();

private:
	// Отмечен или нет.
	bool checked = false;

	// Children nodes.
	std::vector<std::shared_ptr<TreeItem>> children;
};


class TreeView : public clan::ScrollView
{
public:
	TreeView();
	~TreeView();

	// Передать иерархию элементов дереву. Корневой элемент невидим, отображение начинается с его детей.
	void setRootItem(std::shared_ptr<TreeItem> item);

	// Получить корневой элемент дерева.
	std::shared_ptr<TreeItem> getRootItem() { return rootItem; }

	/// Обработчик изменения отметки - установки и снятия галочки.
	std::function<void(TreeItem &item)> func_check_changed;

	void layout_children(clan::Canvas &canvas) override;

private:
	// Корневой невидимый элемент, необходим для того, чтобы можно было сделать несколько ветвей первого уровня.
	std::shared_ptr<TreeItem> rootItem;

	// Добавляет к списку детей указанного элемента с большим отступом.
	void addTreeNodeChildren(int margin, std::shared_ptr<TreeItem> item);

	// Обработчик переключения видимости.
	void onCheckChanged(TreeItem &item);
};