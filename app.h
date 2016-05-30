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
	void on_input_down(const clan::KeyEvent &e);
	void on_window_close();
	void on_mouse_down(const clan::InputEvent &key);
	void on_menuButton_down();


	clan::Canvas canvas;

	// Надпись для отображения FPS
	int lastFPS;
	std::shared_ptr<clan::LabelView> labelFPS;

	// Флаг завершения работы программы.
	bool quit = false;

	// Флаги для переключения в оконный/полноэкранный режим.
	bool fullscreen_requested = false;
	bool is_fullscreen = false;

	clan::UIThread ui_thread;
	std::shared_ptr<clan::TopLevelWindow> window;

	// Окно настроек
	std::shared_ptr<WindowsSettings> window_settings;

	// Счётчик времени.
	clan::GameTime game_time;


	// Кнопка меню - если нажата, то отображается панель настроек.
	std::shared_ptr<clan::ButtonView> menu_button;
};

