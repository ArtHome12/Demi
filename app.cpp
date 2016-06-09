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
#include "Theme/theme.h"
#include "settings_storage.h"
#include "earth.h"
#include "model_render.h"
#include "app.h"


// Высота строки с панелью меню.
const int cTopMenuHeight = 30;

// Положение кнопки меню
const int cMenuButtonTop = 5;
const int cMenuButtonLeft = 5;

const std::string cModelTimeLabel = "Model time:"; 

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

	// Настройки программы
	SettingsStorage settings;

	// Create a window:
	clan::DisplayWindowDescription desc;
	desc.set_title("Demi: Hello World");
	desc.set_allow_resize(true);
	desc.set_position(settings.getMainWindowPosition(), false);
	desc.set_visible(false);
	desc.set_fullscreen(settings.getIsFullScreen());
	pWindow = std::make_shared<clan::TopLevelWindow>(desc);
	const std::shared_ptr<clan::View> pRootView = pWindow->root_view();

	// Дочерние панели распологаются в столбик
	pRootView->style()->set("flex-direction: column");

	// Обработчики событий.
	pRootView->slots.connect(pRootView->sig_close(), [&](clan::CloseEvent &e) { on_window_close(); });
	pRootView->slots.connect(pRootView->sig_key_press(), [&](clan::KeyEvent &e) { on_input_down(e); });
	
	// Без вызова этой функции события клавиатуры не приходят.
	pRootView->set_focus();
	
	canvas = pRootView->canvas();

	// Иконки приложения.
	pWindow->display_window().set_small_icon(clan::PixelBuffer("Flower16.png", doc.get_file_system()));
	pWindow->display_window().set_large_icon(clan::PixelBuffer("Flower32.png", doc.get_file_system()));

	// Панель меню и информации в вехней части окна
	auto pTopPanel = std::make_shared<clan::View>();
	pTopPanel->style()->set("background-color: darkgray");
	pTopPanel->style()->set("flex-direction: row");
	pTopPanel->style()->set("height: 30px");
	pRootView->add_child(pTopPanel);

	// Кнопка в панели меню
	pMenuButton = Theme::create_button();
	pMenuButton->style()->set("margin: 3px");
	pMenuButton->style()->set("width: 23px");
	pMenuButton->set_sticky(true);
	pMenuButton->image_view()->set_image(clan::Image(canvas, "Options.png", doc.get_file_system()));
	pMenuButton->image_view()->style()->set("padding: 0 3px");
	pMenuButton->func_clicked() = clan::bind_member(this, &App::on_menuButton_down);
	pTopPanel->add_child(pMenuButton);

	// Надпись для отображения FPS
	pLabelFPS = std::make_shared<clan::LabelView>();
	pLabelFPS->style()->set("color: white");
	pLabelFPS->style()->set("flex: none");
	pLabelFPS->style()->set("margin: 8px");
	pLabelFPS->style()->set("width: 60px");
	pLabelFPS->style()->set("font: 12px 'tahoma'");
	//pLabelFPS->style()->set("border: 1px solid #DD3B2A");
	pTopPanel->add_child(pLabelFPS);

	// Подпись для отображения времени модели
	auto pLabelModelTimeTitle = std::make_shared<clan::LabelView>();
	pLabelModelTimeTitle->style()->set("color: white");
	pLabelModelTimeTitle->style()->set("flex: none");
	pLabelModelTimeTitle->style()->set("margin: 8px");
	pLabelModelTimeTitle->style()->set("font: 12px 'tahoma'");
	//pLabelModelTimeTitle->style()->set("border: 1px solid #003B2A");
	pLabelModelTimeTitle->set_text(cModelTimeLabel);
	pTopPanel->add_child(pLabelModelTimeTitle);

	// Надпись для отображения времени модели
	pLabelModelTime = std::make_shared<clan::LabelView>();
	pLabelModelTime->style()->set("color: white");
	pLabelModelTime->style()->set("flex: none");
	pLabelModelTime->style()->set("margin: 8px");
	pLabelModelTime->style()->set("width: 60px");
	pLabelModelTime->style()->set("font: 12px 'tahoma'");
	pTopPanel->add_child(pLabelModelTime);

	// Кнопка-надпись левого верхнего угла координат мира.
	pButtonTopLeftModelCoordinate = Theme::create_button();
	pButtonTopLeftModelCoordinate->style()->set("flex: none");
	pButtonTopLeftModelCoordinate->style()->set("margin: 3px");
	pButtonTopLeftModelCoordinate->style()->set("width: 120px");
	pButtonTopLeftModelCoordinate->label()->style()->set("font: 12px 'tahoma';");
	pButtonTopLeftModelCoordinate->label()->set_text("X:Y 0:0");
	pButtonTopLeftModelCoordinate->func_clicked() = clan::bind_member(this, &App::on_menuTopLeftModelButton_down);
	pTopPanel->add_child(pButtonTopLeftModelCoordinate);

	// Кнопка-надпись масштаба координат мира.
	pButtonScaleModel = Theme::create_button();
	pButtonScaleModel->style()->set("flex: none");
	pButtonScaleModel->style()->set("margin: 3px");
	pButtonScaleModel->style()->set("width: 120px");
	pButtonScaleModel->label()->style()->set("font: 12px 'tahoma';");
	pButtonScaleModel->label()->set_text("Scale 1.0");
	pButtonScaleModel->func_clicked() = clan::bind_member(this, &App::on_menuScaleModelButton_down);
	pTopPanel->add_child(pButtonScaleModel);


	// Окно настроек
	pWindowSettings = std::make_shared<WindowsSettings>(canvas);
	pWindowSettings->set_hidden(true);
	pWindow->root_view()->add_child(pWindowSettings);

	// Окно модели
	pModelRender = std::make_shared<ModelRender>();
	pModelRender->style()->set("flex: auto");
	pWindow->root_view()->add_child(pModelRender);

	pMenuButton->set_pressed(true);
	on_menuButton_down();

	// Показываем окно в состоянии, в котором оно было при закрытии (свёрнуто, максимизировано и т.д.)
	pWindow->show(settings.getMainWindowState());

	// Инициализируем счётчик времени.
	game_time.reset();
}

App::~App()
{
	// Настройки программы
	SettingsStorage settings;

	// Для удобства.
	clan::DisplayWindow &dw = pWindow->display_window();

	// Определим состояние окна в формате clan::WindowShowType.
	clan::WindowShowType state = clan::WindowShowType::show_default;
	if (dw.is_maximized())
		state = clan::WindowShowType::maximize;
	else if (dw.is_minimized())
		state = clan::WindowShowType::minimize;

	// Сохраняем местоположение и состояние главного окна.
	settings.setMainWindowPositionAndState(dw.get_geometry(), state, dw.is_fullscreen());
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
		pWindow->display_window().toggle_fullscreen();
	}
	
	// Выведем скорость обновления экрана, но только если она изменилась.
	//
	int fps = int(game_time.get_updates_per_second());
	if (lastFPS != fps) {
		lastFPS = fps;
		std::string fpsStr = clan::StringHelp::int_to_text(fps) + " fps";
		pLabelFPS->set_text(fpsStr, true);
	}

	// Истина, если что-либо в модели изменилось и необходима перерисовка.
	bool isDirty = false;

	// Выведем координаты левого верхнего угла мира.
	const clan::Pointf &topLeftWorld = pModelRender->getTopLeftWorld();
	if (lastTopLeftWorld != topLeftWorld) {
		lastTopLeftWorld = topLeftWorld;
		pButtonTopLeftModelCoordinate->label()->set_text("X:Y "
			+ clan::StringHelp::int_to_text(int(topLeftWorld.x)) + ":"
			+ clan::StringHelp::int_to_text(int(topLeftWorld.y)), true);
		isDirty = true; // Пометим необхдимость перерисовать модель.
	}

	// Выведем масштаб координат мира.
	float scaleWorld = pModelRender->getScaleWorld();
	if (lastScaleWorld != scaleWorld) {
		lastScaleWorld = scaleWorld;
		pButtonScaleModel->label()->set_text("Scale "	+ clan::StringHelp::float_to_text(scaleWorld, 3, false), true);
		isDirty = true; // Пометим необхдимость перерисовать модель.
	}

	// Отрисуем содержимое окна с моделью.
	DemiTime modelTime = globalEarth.getModelTime();
	if (lastModelTime != modelTime) {
		lastModelTime = modelTime;
		pLabelModelTime->set_text(modelTime.getDateStr(), true);
		isDirty = true; // Пометим необхдимость перерисовать модель.
	}

	if (isDirty) 
		pModelRender->draw(canvas);

	//pWindow->display_window().flip();
	pWindow->display_window().flip(0);

	return !quit;
}

// A key was pressed
void App::on_input_down(const clan::KeyEvent &e)
{
	if (e.key() == clan::Key::escape)
	{
		quit = true;
	}
	else if (e.key() == clan::Key::f)
	{
		fullscreen_requested = !fullscreen_requested;
	}
	else if (e.key() == clan::Key::f2)
	{
		pModelRender->toggleIlluminated();
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
	// Показываем или прячем в клиентской области окно настроек.
	pWindowSettings->set_hidden(!pMenuButton->pressed());
}

void App::on_menuTopLeftModelButton_down()
{
	// Reset top left coordinate of the world.
	pModelRender->setTopLeftWorld(clan::Pointf());
}

void App::on_menuScaleModelButton_down()
{
	// Reset the scale.
	pModelRender->setScaleWorld(1.0f);
}

