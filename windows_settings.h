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

	// Завершающая часть настройки, которой требуется окно.
	void initWindow(clan::Canvas& canvas);

	// Для первоначальной загрузки модели.
	void finishInit(clan::WindowManager* windowManager);

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

	// Кнопка сохранения модели.
	std::shared_ptr<clan::ButtonView> pButtonSave;

	// Кнопка сохранения модели под новым именем.
	std::shared_ptr<clan::ButtonView> pButtonSaveAs;

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
	std::shared_ptr<TreeView> pTreeViewElements;
	std::shared_ptr<TreeView> pTreeViewSpecies;

	// Панель под количества элементов.
	std::shared_ptr<View> panelElemAmounts;
	std::shared_ptr<View> panelOrganismAmounts;

	// Количество прошедших секунд с момента старта программы.
	size_t secondsElapsed = 0;

	// Кешированный префикс для надписи для живых организмов, для ускорения.
	std::string cachedAnimalPrefixLabel;

	// Для быстрого и изолированного от расчётного потока обновления количеств сохраним 

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
	void onTreeElementsCheckChanged(TreeItem &item);
	void onTreeSpeciesCheckChanged(TreeItem &item);

	// Сохраняет новое имя модели и обновляет надпись на экране.
	void setModelFilename(const std::string &newName);

	// Обновляет дерево с галочками видимости элементов на основании значений в модели.
	void initElemVisibilityTree();

	// В отличие от обычной загрузки надо сохранить прежние значения видимости.
	void initElemVisibilityTreeAfterRestart();

	// Обновляет отображение организмов (которые меняются в процессе расчёта).
	void initAnimalVisibility();
	void doInitAnimalVisibility(std::shared_ptr<demi::GenotypesTree> treeNode, std::shared_ptr<TreeItem> item);

	// Создаёт надпись.
	std::shared_ptr<clan::LabelView> createLabelForAmount(std::string text);

	clan::WindowManager* wManager;

	// Выкопированный список элементов с дерева для использования при обновлении количеств.
	std::vector<GenotypesTreeHelperItem*> treeBackup;

	// Сохраняет модель с показом диалогового окна.
	void saveModel(const std::string& filename);

	// Загружает модель с показом диалогового окна.
	void loadModel(const std::string& filename);

	// Обновляет надписи с количествами.
	void updateAmounts();

	// Показывает сообщение об ошибке.
	void showError(const std::string& errMessage);
};

