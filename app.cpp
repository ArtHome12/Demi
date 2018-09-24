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
#include "world.h"
#include "model_render.h"
#include "app.h"


// Высота строки с панелью меню.
const int cTopMenuHeight = 30;

// Положение кнопки меню
const int cMenuButtonTop = 5;
const int cMenuButtonLeft = 5;

// Строковые ресурсы
auto cMainWindowTitle = "AppMainWindowTitle";
auto cModelTimeLabel = "AppModelTimeLabel";
auto cScaleLabel = "AppScaleLabel";

// Инициализируем библиотеку.
clan::ApplicationInstance<App> clanapp;

App::App()
{
#if defined(WIN32) && !defined(__MINGW32__)
	clan::D3DTarget::set_current();
#else
	clan::OpenGLTarget::set_current();
#endif

	// Звуковая подсистема.
	pSoundOutput = std::make_shared<clan::SoundOutput>(44100, 192);

	// Ресурсы необходимо создавать после инициализации графической подсистемы.
	pSettings = std::make_shared<SettingsStorage>();

	// Mark this thread as the UI thread
	ui_thread = clan::UIThread(pSettings->resManager);

	// Доинициализируем глобальный мир.
	globalWorld.setSettingsStorage(pSettings.get());

	// Create a window:
	clan::DisplayWindowDescription desc;
	desc.set_title(pSettings->LocaleStr(cMainWindowTitle));
	desc.set_allow_resize(true);
	desc.set_position(pSettings->getMainWindowPosition(), false);
	desc.set_visible(false);
	desc.set_fullscreen(pSettings->getIsFullScreen());
	pWindow = std::make_shared<clan::TopLevelWindow>(desc);
	const std::shared_ptr<clan::View> pRootView = pWindow->root_view();

	// Дочерние панели распологаются в столбик
	//pRootView->style()->set("flex-direction: column; padding: 11px;");
	pRootView->style()->set("flex-direction: column;");

	// Обработчики событий.
	pRootView->slots.connect(pRootView->sig_close(), [&](clan::CloseEvent &e) { on_window_close(); });
	pRootView->slots.connect(pRootView->sig_key_press(), [&](clan::KeyEvent &e) { on_input_down(e); });
	
	// Без вызова этой функции события клавиатуры не приходят.
	pRootView->set_focus();
	
	canvas = pRootView->canvas();

	// Иконки приложения.
	pWindow->display_window().set_small_icon(clan::PixelBuffer("Flower16.png", pSettings->fileResDoc.get_file_system()));
	pWindow->display_window().set_large_icon(clan::PixelBuffer("Flower32.png", pSettings->fileResDoc.get_file_system()));

	// Панель меню и информации в вехней части окна
	auto pTopPanel = std::make_shared<clan::View>();
	pTopPanel->style()->set("background-color: darkgray; flex-direction: row; height: 30px");
	pRootView->add_child(pTopPanel);

	// Кнопка в панели меню
	pMenuButton = Theme::create_button();
	pMenuButton->style()->set("margin: 3px; width: 23px");
	pMenuButton->set_sticky(true);
	pMenuButton->image_view()->set_image(clan::Image(canvas, "Options.png", pSettings->fileResDoc.get_file_system()));
	pMenuButton->image_view()->style()->set("padding: 0 3px");
	pMenuButton->func_clicked() = clan::bind_member(this, &App::on_menuButton_down);
	pTopPanel->add_child(pMenuButton);

	// Надпись для отображения FPS
	pLabelFPS = std::make_shared<clan::LabelView>();
	pLabelFPS->style()->set("color: white; flex: none; margin: 8px; width: 60px; font: 12px 'tahoma'");
	//pLabelFPS->style()->set("border: 1px solid #DD3B2A");
	pTopPanel->add_child(pLabelFPS);

	// Подпись для отображения времени модели
	auto pLabelModelTimeTitle = std::make_shared<clan::LabelView>();
	pLabelModelTimeTitle->style()->set("color: white; flex: none; margin: 8px; font: 12px 'tahoma'");
	//pLabelModelTimeTitle->style()->set("border: 1px solid #003B2A");
	pLabelModelTimeTitle->set_text(pSettings->LocaleStr(cModelTimeLabel));
	pTopPanel->add_child(pLabelModelTimeTitle);

	// Надпись для отображения времени модели
	pLabelModelTime = std::make_shared<clan::LabelView>();
	pLabelModelTime->style()->set("color: white; flex: none; margin: 8px; width: 120px; font: 12px 'tahoma'");
	pTopPanel->add_child(pLabelModelTime);

	// Кнопка-надпись левого верхнего угла координат мира.
	pButtonTopLeftModelCoordinate = Theme::create_button();
	pButtonTopLeftModelCoordinate->style()->set("flex: none; margin: 3px; width: 120px");
	pButtonTopLeftModelCoordinate->label()->style()->set("font: 12px 'tahoma'");
	pButtonTopLeftModelCoordinate->label()->set_text("X:Y 0:0");
	pButtonTopLeftModelCoordinate->func_clicked() = clan::bind_member(this, &App::on_menuTopLeftModelButton_down);
	pTopPanel->add_child(pButtonTopLeftModelCoordinate);

	// Кнопка-надпись масштаба координат мира.
	pButtonScaleModel = Theme::create_button();
	pButtonScaleModel->style()->set("flex: none; margin: 3px; width: 120px");
	pButtonScaleModel->label()->style()->set("font: 12px 'tahoma';");
	pButtonScaleModel->label()->set_text("Scale 1.0");
	pButtonScaleModel->func_clicked() = clan::bind_member(this, &App::on_menuScaleModelButton_down);
	pTopPanel->add_child(pButtonScaleModel);

	// Кнопка постоянного освещения мира.
	pButtonIlluminatedModel = Theme::create_button();
	pButtonIlluminatedModel->style()->set("margin: 3px; width: 23px");
	pButtonIlluminatedModel->set_sticky(true);
	pButtonIlluminatedModel->image_view()->set_image(clan::Image(canvas, "IlluminateOn.png", pSettings->fileResDoc.get_file_system()));
	pButtonIlluminatedModel->image_view()->style()->set("padding: 0 3px");
	pButtonIlluminatedModel->func_clicked() = clan::bind_member(this, &App::on_menuIlluminatedModelButton_down);
	pTopPanel->add_child(pButtonIlluminatedModel);

	// Окно настроек
	pWindowSettings = std::make_shared<WindowsSettings>(canvas, pSettings);
	pWindowSettings->set_hidden(true);
	pRootView->add_child(pWindowSettings);

	// Окно модели
	pModelRender = std::make_shared<ModelRender>(pSettings);
	pModelRender->style()->set("flex: auto; background-color: black;");
	pRootView->add_child(pModelRender);

	// Отобразим окно настроек.
	if (pSettings->getTopMenuIsSettingsWindowVisible()) {
		pMenuButton->set_pressed(true);
		on_menuButton_down();
	}

	// Настроим освещённость.
	if (pSettings->getTopMenuIsModelIlluminated()) {
		pButtonIlluminatedModel->set_pressed(true);
		on_menuIlluminatedModelButton_down();
	}

	// Строковые ресурсы.
	scaleLabelTemplate = pSettings->LocaleStr(cScaleLabel);

	// Показываем окно в состоянии, в котором оно было при закрытии (свёрнуто, максимизировано и т.д.)
	pWindow->show(pSettings->getMainWindowState());

	// Инициализируем счётчик времени.
	game_time.reset();
}

App::~App()
{
	// Для удобства.
	clan::DisplayWindow &dw = pWindow->display_window();

	// Определим состояние окна в формате clan::WindowShowType.
	clan::WindowShowType state = clan::WindowShowType::show_default;
	if (dw.is_maximized())
		state = clan::WindowShowType::maximize;
	else if (dw.is_minimized())
		state = clan::WindowShowType::minimize;

	// Сохраняем местоположение и состояние главного окна.
	pSettings->setMainWindowSettings(dw.get_geometry(),
		state, 
		dw.is_fullscreen());
}



bool App::update()
{
	// Check for fullscreen switch.
	if (fullscreen_requested != is_fullscreen)
	{
		is_fullscreen = fullscreen_requested;
		pWindow->display_window().toggle_fullscreen();
	}

	// Если окно свёрнуто, ничего не делаем.
	if (!pWindow->display_window().is_minimized()) {

		// Выведем скорость обновления экрана, но только если она изменилась, так как операция медленная.
		int fps = int(game_time.get_updates_per_second());
		if (lastFPS != fps) {
			lastFPS = fps;
			std::string fpsStr = clan::StringHelp::int_to_text(fps) + " fps";
			pLabelFPS->set_text(fpsStr, true);
		}

		// Выведем координаты левого верхнего угла мира.
		const clan::Point &topLeftWorld = globalWorld.getAppearanceTopLeft();
		if (lastTopLeftWorld != topLeftWorld) {
			lastTopLeftWorld = topLeftWorld;
			pButtonTopLeftModelCoordinate->label()->set_text("X:Y "
				+ clan::StringHelp::int_to_text(int(topLeftWorld.x)) + ":"
				+ clan::StringHelp::int_to_text(int(topLeftWorld.y)), true);
		}

		// Выведем масштаб координат мира.
		float scaleWorld = globalWorld.getAppearanceScale();
		if (fabs(lastScaleWorld - scaleWorld) > 0.0001f) {
			lastScaleWorld = scaleWorld;
			pButtonScaleModel->label()->set_text(scaleLabelTemplate + clan::StringHelp::float_to_text(scaleWorld, 3, false), true);
		}

		// Проверим, не изменилась ли освещённость.
		bool illuminatedWorld = pModelRender->getIlluminatedWorld();
		if (lastIlluminatedWorld != illuminatedWorld) {
			lastIlluminatedWorld = illuminatedWorld;
		}

		// Выведем время модели.
		const demi::DemiTime& modelTime = globalWorld.getModelTime();
		if (lastModelTime != modelTime) {
			lastModelTime = modelTime;
			pLabelModelTime->set_text(modelTime.getDateStr(), true);
		}

		// Отрисовываем модель.
		pModelRender->draw_without_layout();
	}
	else {
		// Если окно свёрнуто, отдохнём.
		clan::System::sleep(100);

		// Засчитаем отдых за обновление фрейма для того, чтобы автосохранение каждый час работало. Правильность
		// отображения fps для свёрнутого окна роли не играет.
		game_time.update();
	}

	// Обновим счётчик времени, если действительно обновилось отображение модели. Это нужно для корректного отображения fps,
	// иначе fps формально высок, а отклик на изменение видимости тормозной, то есть реальный.
	if (pModelRender->getIsFrameUpdated())
		game_time.update();

	// Уведомляем окно настроек об отрисовке (для проверки наступления момента автосохранения модели).
	pWindowSettings->modelRenderNotify(size_t(game_time.get_current_time()));

	// Выводим заэкранный буфер на экран.
	pWindow->display_window().flip();

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
		// Переключаем подсветку модели.
		pButtonIlluminatedModel->set_pressed(!pButtonIlluminatedModel->pressed());
		on_menuIlluminatedModelButton_down();
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
	// Нажата ли кнопка отображения панели с настройками.
	bool isPressed = pMenuButton->pressed();
	
	// Показываем или прячем в клиентской области окно настроек.
	pWindowSettings->set_hidden(!isPressed);

	// Сохраняем настройку.
	pSettings->setTopMenuIsSettingsWindowVisible(isPressed);
}

void App::on_menuTopLeftModelButton_down()
{
	// Reset top left coordinate of the world.
	globalWorld.setAppearanceTopLeft(clan::Point());
}

void App::on_menuScaleModelButton_down()
{
	// Reset the scale.
	globalWorld.setAppearanceScale(1.0f);
}

void App::on_menuIlluminatedModelButton_down()
{
	// Показываем или прячем в клиентской области окно настроек.
	pModelRender->setIlluminatedWorld(pButtonIlluminatedModel->pressed());
}
