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

class MainWindow : public clan::WindowController
{
public:
	MainWindow();
	~MainWindow();

	// ����������� ����� ���������, ������� ��������� ����.
	void initWindow(clan::WindowManager* wManager);

private:
	friend class App;

	// ������� ��� ����������� FPS
	std::shared_ptr<clan::LabelView> pLabelFPS;

	// ������� ��� ����������� ������� ������.
	demi::DemiTime lastModelTime;
	std::shared_ptr<clan::LabelView> pLabelModelTime;

	// ������-������� ������ �������� ���� ��������� ����.
	clan::Point lastTopLeftWorld;
	std::shared_ptr<clan::ButtonView> pButtonTopLeftModelCoordinate;

	// ������-������� �������� ��������� ����.
	float lastScaleWorld = 0.0f;
	std::string scaleLabelTemplate;
	std::shared_ptr<clan::ButtonView> pButtonScaleModel;

	// ������-������� ����������� ��������� ����.
	bool lastIlluminatedWorld = false;
	std::shared_ptr<clan::ButtonView> pButtonIlluminatedModel;

	// ����� ��� ������������ � �������/������������� �����.
	bool fullscreen_requested = false;
	bool is_fullscreen = false;

	// ������ ���� - ���� ������, �� ������������ ������ ��������.
	std::shared_ptr<clan::ButtonView> pMenuButton;

	// ���� ��������
	std::shared_ptr<WindowsSettings> pWindowSettings;

	// ���� ������
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

	// ��������� ���������
	std::shared_ptr<SettingsStorage> pSettings;

	clan::UIThread ui_thread;

	// ������� ����.
	std::shared_ptr<MainWindow> pMainWindow;

	// ������� �������.
	clan::GameTime game_time;

	// �������� ����������.
	std::shared_ptr<clan::SoundOutput> pSoundOutput;

	clan::WindowManager windowManager;
};

