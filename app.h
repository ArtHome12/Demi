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

class App : public clan::Application
{
public:
	App();
	~App();
	bool update() override;

private:

	clan::Canvas canvas;

	// Настройки программы
	std::shared_ptr<SettingsStorage> pSettings;

	// Надпись для отображения FPS
	int lastFPS;
	std::shared_ptr<clan::LabelView> pLabelFPS;

	// Надпись для отображения времени модели.
	DemiTime lastModelTime;
	std::shared_ptr<clan::LabelView> pLabelModelTime;

	// Кнопка-надпись левого верхнего угла координат мира.
	clan::Pointf lastTopLeftWorld;
	std::shared_ptr<clan::ButtonView> pButtonTopLeftModelCoordinate;

	// Кнопка-надпись масштаба координат мира.
	float lastScaleWorld = 0.0f;
	std::string scaleLabelTemplate;
	std::shared_ptr<clan::ButtonView> pButtonScaleModel;

	// Кнопка-надпись постоянного освещения мира.
	bool lastIlluminatedWorld = false;
	std::shared_ptr<clan::ButtonView> pButtonIlluminatedModel;

	// Флаг завершения работы программы.
	bool quit = false;

	// Флаги для переключения в оконный/полноэкранный режим.
	bool fullscreen_requested = false;
	bool is_fullscreen = false;

	clan::UIThread ui_thread;
	std::shared_ptr<clan::TopLevelWindow> pWindow;

	// Окно настроек
	std::shared_ptr<WindowsSettings> pWindowSettings;

	// Окно модели
	std::shared_ptr<ModelRender> pModelRender;

	// Счётчик времени.
	clan::GameTime game_time;

	// Кнопка меню - если нажата, то отображается панель настроек.
	std::shared_ptr<clan::ButtonView> pMenuButton;

	// Звуковая подсистема.
	std::shared_ptr<clan::SoundOutput> pSoundOutput;

	void on_input_down(const clan::KeyEvent &e);
	void on_window_close();
	void on_mouse_down(const clan::InputEvent &key);
	void on_menuButton_down();
	void on_menuTopLeftModelButton_down();
	void on_menuScaleModelButton_down();
	void on_menuIlluminatedModelButton_down();
};

