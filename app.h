/* ===============================================================================
Моделирование эволюции живого мира.
Главное окно программы.
25 may 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

class MainWindow : public clan::WindowController
{
public:
	MainWindow();
	~MainWindow();

	// Завершающая часть настройки, которой требуется окно.
	void initWindow(clan::WindowManager* wManager);

private:
	friend class App;

	// Надпись для отображения FPS
	std::shared_ptr<clan::LabelView> pLabelFPS;

	// Надпись для отображения времени модели.
	demi::DemiTime lastModelTime;
	std::shared_ptr<clan::LabelView> pLabelModelTime;

	// Кнопка-надпись левого верхнего угла координат мира.
	clan::Point lastTopLeftWorld;
	std::shared_ptr<clan::ButtonView> pButtonTopLeftModelCoordinate;

	// Кнопка-надпись масштаба координат мира.
	float lastScaleWorld = 0.0f;
	std::string scaleLabelTemplate;
	std::shared_ptr<clan::ButtonView> pButtonScaleModel;

	// Кнопка-надпись постоянного освещения мира.
	bool lastIlluminatedWorld = false;
	std::shared_ptr<clan::ButtonView> pButtonIlluminatedModel;

	// Флаги для переключения в оконный/полноэкранный режим.
	bool fullscreen_requested = false;
	bool is_fullscreen = false;

	// Кнопка меню - если нажата, то отображается панель настроек.
	std::shared_ptr<clan::ButtonView> pMenuButton;

	// Окно настроек
	std::shared_ptr<WindowsSettings> pWindowSettings;

	// Окно модели
	std::shared_ptr<ModelRender> pModelRender;

	void on_input_down(const clan::KeyEvent &e);
	void on_window_close();
	void on_mouse_down(const clan::InputEvent &key);
	void on_menuButton_down();
	void on_menuTopLeftModelButton_down();
	void on_menuScaleModelButton_down();
	void on_menuIlluminatedModelButton_down();
};

class App : public clan::Application
{
public:
	App();
	~App();
	bool update() override;

private:

	// Настройки программы
	std::shared_ptr<SettingsStorage> pSettings;

	clan::UIThread ui_thread;

	// Главное окно.
	std::shared_ptr<MainWindow> pMainWindow;

	// Счётчик времени.
	clan::GameTime game_time;

	// Звуковая подсистема.
	std::shared_ptr<clan::SoundOutput> pSoundOutput;

	clan::WindowManager windowManager;
};

