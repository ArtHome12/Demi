/* ===============================================================================
Моделирование эволюции живого мира.
Окно с настройками.
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

	// Для получения уведомлений об отработке основного цикла (для возможного автосохранения модели, обновления количеств).
	void modelRenderNotify(size_t secondsElapsed);

private:
	// Название текущей модели
	std::string modelFilename;
	std::shared_ptr<clan::LabelView> pLabelModelName;

	// Кнопка создания новой модели.
	std::shared_ptr<clan::ButtonView> pButtonNew;

	// Кнопка открытия модели.
	std::shared_ptr<clan::ButtonView> pButtonOpen;

	// Кнопка перезапуска модели.
	std::shared_ptr<clan::ButtonView> pButtonRestart;

	// Кнопка запуска или приостановки расчёта.
	std::shared_ptr<clan::ButtonView> pButtonRunPause;

	// Чекбокс автозапуска модели
	std::shared_ptr<clan::CheckBoxView> pCBAutoRun;

	// Чекбокс автосохранения модели при выходе из программы.
	std::shared_ptr<clan::CheckBoxView> pCBAutoSave;

	// Чекбокс периодического (ежечасного) автосохранения модели.
	std::shared_ptr<clan::CheckBoxView> pCBAutoSaveHourly;

	// Дерево с галочками видимости элементов.
	std::shared_ptr<TreeView> pTreeView;

	// Панель под количества элементов.
	std::shared_ptr<View> panelElemAmounts;

	// Настройки.
	std::shared_ptr<SettingsStorage> pSettings;

	// Количество прошедших секунд с момента старта программы.
	size_t secondsElapsed = 0;

	// Обработчики событий
	void onButtondownNew();
	void onButtondownOpen();
	void onButtondownSave();
	void onButtondownSaveAs();
	void onButtondownRunPause();
	void onButtondownRestart();
	void onCBAutoRunToggle();
	void onCBAutoSaveToggle();
	void onCBAutoSaveHourlyToggle();

	// Обработчик события, вызываемый после переключения галочки на элементе древовидного списка.
	void onTreeCheckChanged(TreeItem &item);

	// Сохраняет новое имя модели и обновляет надпись на экране.
	void set_modelFilename(const std::string &newName);

	// Обновляет дерево с галочками видимости элементов на основании значений в модели.
	void initElemVisibilityTree();

	// Есть особенности по сравнению с обычной загрузкой.
	void initElemVisibilityTreeAfterRestart();

	// Создаёт надпись.
	std::shared_ptr<clan::LabelView> createLabelForAmount(std::string text);
};

