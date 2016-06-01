/* ===============================================================================
������������� �������� ������ ����.
������� ���� ���������.
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

	// ������� ��� ����������� FPS
	int lastFPS;
	std::shared_ptr<clan::LabelView> pLabelFPS;

	// ������� ��� ����������� ������� ������.
	DemiTime lastModelTime;
	std::shared_ptr<clan::LabelView> pLabelModelTime;

	// ���� ���������� ������ ���������.
	bool quit = false;

	// ����� ��� ������������ � �������/������������� �����.
	bool fullscreen_requested = false;
	bool is_fullscreen = false;

	clan::UIThread ui_thread;
	std::shared_ptr<clan::TopLevelWindow> pWindow;

	// ���� ��������
	std::shared_ptr<WindowsSettings> pWindowSettings;

	// ������� �������.
	clan::GameTime game_time;

	// ������������ ��� ��������� ������������ ��������.
	clan::Font font;

	// ������ ���� - ���� ������, �� ������������ ������ ��������.
	std::shared_ptr<clan::ButtonView> pMenuButton;
};

