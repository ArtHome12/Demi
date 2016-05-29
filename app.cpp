/* ===============================================================================
Моделирование эволюции живого мира.
Главное окно программы.
25 may 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "windows_settings.h"
#include "app.h"
#include "Theme/theme.h"

// Высота строки с панелью меню.
const int cTopMenuHeight = 30;

// Положение кнопки меню
const int cMenuButtonTop = 5;
const int cMenuButtonLeft = 5;

clan::ApplicationInstance<App> clanapp;

App::App()
{
#if defined(WIN32) && !defined(__MINGW32__)
	clan::D3DTarget::set_current();
#else
	clan::OpenGLTarget::set_current();
#endif

	// Create a source for our resources
	clan::FileResourceDocument doc(clan::FileSystem("ThemeAero"));
	clan::ResourceManager resources = clan::FileResourceManager::create(doc);

	// Mark this thread as the UI thread
	ui_thread = clan::UIThread(resources);

	// Create a window:
	clan::DisplayWindowDescription desc;
	desc.set_title("Demi: Hello World");
	desc.set_allow_resize(true);
	//desc.set_size(Sizef(640, 600), false);
	window = std::make_shared<clan::TopLevelWindow>(desc);
	const std::shared_ptr<clan::View> view = window->root_view();

	// Обработчики событий.
	view->slots.connect(view->sig_close(), [&](clan::CloseEvent &e) { on_window_close(); });
	view->slots.connect(view->sig_key_press(), [&](clan::KeyEvent &e) { on_input_down(e); });
	
	// Без этого события клавиатуры не приходят
	view->set_focus();
	
	canvas = view->canvas();

	// Иконки приложения.
	window->display_window().set_small_icon(clan::PixelBuffer("ThemeAero/Flower16.png"));
	window->display_window().set_large_icon(clan::PixelBuffer("ThemeAero/Flower32.png"));

	// Дочерние панели распологаются в столбик
	view->style()->set("flex-direction: column");

	// Панель меню и информации в вехней части окна
	auto topPanel = std::make_shared<clan::View>();
	topPanel->style()->set("background: darkgray");
	topPanel->style()->set("flex-direction: row");
	topPanel->style()->set("height: 30px");
	view->add_child(topPanel);

	// Кнопка в панели меню
	menu_button = Theme::create_button();
	menu_button->style()->set("margin: 3px");
	menu_button->style()->set("width: 23px");
	menu_button->set_sticky(true);
	menu_button->image_view()->set_image(clan::Image(canvas, "ThemeAero/Options.png"));
	menu_button->image_view()->style()->set("padding: 0 3px");
	menu_button->func_clicked() = clan::bind_member(this, &App::on_menuButton_down);
	topPanel->add_child(menu_button);

	// Надпись для отображения FPS
	labelFPS = std::make_shared<clan::LabelView>();
	labelFPS->style()->set("color: white");
	labelFPS->style()->set("flex: none");
	labelFPS->style()->set("margin: 5px");
	labelFPS->style()->set("font: 12px 'tahoma'");
	topPanel->add_child(labelFPS);

	// Окно настроек
	window_settings = std::make_shared<WindowsSettings>(canvas);

	menu_button->set_pressed(true);
	on_menuButton_down();

	// Инициализируем счётчик времени.
	game_time.reset();
}

bool App::update()
{
	// Обновим счётчик времени.
	game_time.update();

	// Check for fullscreen switch.
	//
	if (fullscreen_requested != is_fullscreen)
	{
		is_fullscreen = fullscreen_requested;
		window->display_window().toggle_fullscreen();
	}
	
	// Выведем скорость обновления экрана, но только если она изменилась - так как каждое изменение текста вызывает InvalidateRect()
	//
	int fps = int(game_time.get_updates_per_second());
	if (lastFPS != fps) {
		lastFPS = fps;
		std::string fpsStr = clan::StringHelp::int_to_text(fps) + " fps";
		labelFPS->set_text(fpsStr);
	}


	// Отрисуем содержимое окна с моделью.


	window->display_window().flip();

	return !quit;
}

// A key was pressed
void App::on_input_down(const clan::KeyEvent &e)
{
	if (e.key() == clan::Key::escape)
	{
		quit = true;
	}

	if ((e.text() == "f") || (e.text() == "F"))
	{
		fullscreen_requested = !fullscreen_requested;
	}
}

// The window was closed
void App::on_window_close()
{
	quit = true;
}

void App::on_mouse_down(const clan::InputEvent &key)
{
	//const char *str = "Unknown";
	//switch (key.id)
	//{
	//case mouse_left:
	//	str = "Left";
	//	break;
	//case mouse_right:
	//	str = "Right";
	//	break;
	//case mouse_middle:
	//	str = "Middle";
	//	break;
	//case mouse_wheel_up:
	//	str = "Up";
	//	break;
	//case mouse_wheel_down:
	//	str = "Down";
	//	break;
	//case mouse_xbutton1:
	//	str = "Button 1";
	//	break;
	//case mouse_xbutton2:
	//	str = "Button 2";
	//	break;
	//default:
	//	break;
	//}
	//create_shooter(key, str, false, true, false);
}

void App::on_menuButton_down()
{
	// Показываем в клиентской области либо окно настроек, либо окно модели.
	if (menu_button->pressed()) 
		window->root_view()->add_child(window_settings);
	else 
		window_settings->remove_from_parent();
}