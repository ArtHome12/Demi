/* ===============================================================================
Моделирование эволюции живого мира.
Древовидный список с чекбоксами.
10 april 2017.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "tree_view.h"
#include "tree_view_impl.h"
#include "Theme/theme.h"

const int cTreeMarginStep = 30;


// Обработчик переключения видимости.
void TreeItem::onStateChanged()
{
	// Переключим состояние. Надежней было бы запросить чекбокс, но это громоздко, простого способа нет.
	checked = !checked;
	
	if (func_check_state_changed != NULL) 
		func_check_state_changed(*this);
}



TreeView::TreeView() : impl(new TreeView_Impl())
{
	impl->treeView = this;
	content_view()->style()->set("flex-direction: column");

	set_focus_policy(clan::FocusPolicy::accept);

	//slots.connect(sig_key_press(), impl.get(), &ListBoxViewImpl::on_key_press);
	//slots.connect(content_view()->sig_pointer_press(), impl.get(), &ListBoxViewImpl::on_pointer_press);
	//slots.connect(content_view()->sig_pointer_release(), impl.get(), &ListBoxViewImpl::on_pointer_release);
}

TreeView::~TreeView()
{
}


std::function<void(TreeItem &item)> &TreeView::func_check_changed()
{
	return impl->func_check_changed;
}

void TreeView::set_root_item(std::shared_ptr<TreeItem> item)
{
	// Сохраняем ссылку на корневой элемент.
	impl->rootItem = item;

	// Удаляем старые чекбоксы из дерева
	auto &views = content_view()->children();
	while (!views.empty())
		views.back()->remove_from_parent();

	// Добавляем детей корневого элемента в дерево.
	impl->addTreeNodeChildren(0, item);
}

/// Получить корневой элемент дерева.
std::shared_ptr<TreeItem> TreeView::get_root_item() 
{ 
	return impl->rootItem; 
}


//void ListBoxView::set_items(const std::vector<std::shared_ptr<View>> &items)
//{
//	impl->selected_item = -1;
//
//	auto views = content_view()->children();
//	while (!views.empty())
//		views.back()->remove_from_parent();
//
//	for (auto &item : items)
//	{
//		content_view()->add_child(item);
//		slots.connect(item->sig_pointer_enter(), impl.get(), &ListBoxViewImpl::on_pointer_enter);
//		slots.connect(item->sig_pointer_leave(), impl.get(), &ListBoxViewImpl::on_pointer_leave);
//	}
//}
//
//int ListBoxView::selected_item() const
//{
//	return impl->selected_item;
//}
//
//void ListBoxView::set_selected_item(int index)
//{
//	if (index == impl->selected_item)
//		return;
//
//	if (index < -1 || index >= (int)content_view()->children().size())
//		throw Exception("Listbox index out of bounds");
//
//	if (impl->selected_item != -1)
//		content_view()->children().at(impl->selected_item)->set_state("selected", false);
//
//	if (index != -1)
//	{
//		if (impl->hot_item == index)
//			impl->set_hot_item(-1);
//
//		auto new_selected_item = content_view()->children().at(index);
//		new_selected_item->set_state("selected", true);
//
//		// Scroll to selected in layout_children(), when geometry will be defined.
//		needScrollToSelected = true;
//	}
//
//	impl->selected_item = index;
//}

void TreeView::layout_children(clan::Canvas &canvas)
{
	// Call parent.
	ScrollView::layout_children(canvas);

	// Content of the ListBoxView.
	auto items(content_view()->children());

	//// Scroll to selected item if it needs.
	//if (needScrollToSelected) {

	//	// Geometry of the selected item.
	//	const Rectf boxOfSelected = items.at(impl->selected_item)->geometry().margin_box();


	//	// Scroll position.
	//	double scrollPos = scrollbar_y_view()->position();

	//	// If the selected item above the visible area, scroll up to selected.
	//	if (boxOfSelected.top < scrollPos)
	//		scrollbar_y_view()->set_position(boxOfSelected.top);
	//	else {
	//		// If the lower part of selected item below the visible area, scroll down.
	//		if (boxOfSelected.bottom > scrollPos + geometry().content_height)
	//			scrollbar_y_view()->set_position(boxOfSelected.bottom - geometry().content_height);
	//	}

	//	// Clear the flag.
	//	needScrollToSelected = false;

	//	// Call parent again to update the scrolled positions.
	//	ScrollView::layout_children(canvas);
	//}

	// Set line_step size equal to the height of first item.
	if (!items.empty()) {
		float height = items.front()->geometry().margin_box().get_height();
		scrollbar_y_view()->set_line_step(height);
	}
}

// Обработчик переключения видимости.
void TreeView_Impl::onCheckChanged(TreeItem &item)
{
	if (func_check_changed != NULL)
		func_check_changed(item);
}



// Добавляет к списку детей указанного элемента с большим отступом.
void TreeView_Impl::addTreeNodeChildren(int margin, std::shared_ptr<TreeItem> item)
{
	for (auto &curItem : item->children)
	{
		auto checkBox = Theme::create_checkbox();
		checkBox->style()->set("margin: 0px " + std::to_string(margin) + "px");
		checkBox->label()->set_text(curItem->caption, true);

		// Если элемент выбран, передадим эту информацию.
		checkBox->set_check(curItem->checked);

		// Обработчик на переключение видимости.
		checkBox->func_state_changed() = clan::bind_member(curItem.get(), &TreeItem::onStateChanged);

		// Для элемента дерева устанавливаем обработчик на дерево.
		curItem->func_check_state_changed = clan::bind_member(this, &TreeView_Impl::onCheckChanged);

		treeView->content_view()->add_child(checkBox);

		// Рекурсивно добавляем его детей с большим отступом.
		addTreeNodeChildren(margin + cTreeMarginStep, curItem);

		//slots.connect(item->sig_pointer_enter(), impl.get(), &ListBoxViewImpl::on_pointer_enter);
		//slots.connect(item->sig_pointer_leave(), impl.get(), &ListBoxViewImpl::on_pointer_leave);
	}

}

