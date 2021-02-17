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
#include "app.h"
#include "windows_settings.h"
#include "Theme/theme.h"
#include "settings_storage.h"
#include "world.h"
#include "model_render.h"


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

MainWindow::MainWindow()
{
	auto pSettings = globalWorld.getSettingsStorage();

	const std::shared_ptr<clan::View> pRootView = root_view();

	// Дочерние панели распологаются в столбик
	pRootView->style()->set("flex-direction: column;");

	// Панель меню и информации в вехней части окна
	auto pTopPanel = std::make_shared<clan::View>();
	pTopPanel->style()->set("background-color: darkgray; flex-direction: row; height: 30px");
	pRootView->add_child(pTopPanel);

	// Кнопка в панели меню
	pMenuButton = Theme::create_button();
	pMenuButton->style()->set("margin: 3px; width: 23px");
	pMenuButton->set_sticky(true);
	pMenuButton->image_view()->style()->set("padding: 0 3px");
	pTopPanel->add_child(pMenuButton);

	// Надпись для отображения FPS/TPS
	pLabelFPS = std::make_shared<clan::LabelView>();
	pLabelFPS->style()->set("color: white; flex: none; margin: 8px; width: 40px; font: 12px 'tahoma'");
	//pLabelFPS->style()->set("border: 1px solid #DD3B2A");
	pTopPanel->add_child(pLabelFPS);

	// Подпись для отображения FPS/TPS
	auto pLabelFPSTitle = std::make_shared<clan::LabelView>();
	pLabelFPSTitle->style()->set("color: white; flex: none; margin: 8px; font: 12px 'tahoma'");
	pLabelFPSTitle->set_text("fps/tps");
	pTopPanel->add_child(pLabelFPSTitle);

	// Подпись для отображения времени модели
	auto pLabelModelTimeTitle = std::make_shared<clan::LabelView>();
	pLabelModelTimeTitle->style()->set("color: white; flex: none; margin: 8px; font: 12px 'tahoma'");
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
	pTopPanel->add_child(pButtonTopLeftModelCoordinate);

	// Кнопка-надпись масштаба координат мира.
	pButtonScaleModel = Theme::create_button();
	pButtonScaleModel->style()->set("flex: none; margin: 3px; width: 120px");
	pButtonScaleModel->label()->style()->set("font: 12px 'tahoma';");
	pButtonScaleModel->label()->set_text("Scale 1.0");
	pTopPanel->add_child(pButtonScaleModel);

	// Кнопка постоянного освещения мира.
	pButtonIlluminatedModel = Theme::create_button();
	pButtonIlluminatedModel->style()->set("margin: 3px; width: 23px");
	pButtonIlluminatedModel->set_sticky(true);
	pButtonIlluminatedModel->image_view()->style()->set("padding: 0 3px");
	pTopPanel->add_child(pButtonIlluminatedModel);

	// Окно настроек
	pWindowSettings = std::make_shared<WindowsSettings>();
	pWindowSettings->set_hidden(true);
	pRootView->add_child(pWindowSettings);

	// Окно модели
	pModelRender = std::make_shared<ModelRender>(pSettings);
	pModelRender->style()->set("flex: auto; background-color: black;");
	pRootView->add_child(pModelRender);

	// Строковые ресурсы.
	scaleLabelTemplate = pSettings->LocaleStr(cScaleLabel);
}

MainWindow::~MainWindow()
{
}

void MainWindow::initWindow(clan::WindowManager* wManager)
{
	auto pSettings = globalWorld.getSettingsStorage();
	auto pRootView = root_view();
	auto canvas = pRootView->canvas();

	// Иконки приложения.
	std::vector<std::string> iconNames{ "ThemeAero/Flower16.png", "ThemeAero/Flower32.png" };
	set_icon(iconNames);

	// Обработчики событий.
	pRootView->slots.connect(pRootView->sig_close(), [&](clan::CloseEvent &e) { on_window_close(); });
	pRootView->slots.connect(pRootView->sig_key_press(), [&](clan::KeyEvent &e) { on_input_down(e); });

	// Без вызова этой функции события клавиатуры не приходят.
	pRootView->set_focus();

	// Доинициализируем элементы интерфейса.
	pMenuButton->image_view()->set_image(clan::Image(canvas, "Options.png", pSettings->fileResDoc.get_file_system()));
	pMenuButton->func_clicked() = clan::bind_member(this, &MainWindow::on_menuButton_down);
	pButtonTopLeftModelCoordinate->func_clicked() = clan::bind_member(this, &MainWindow::on_menuTopLeftModelButton_down);
	pButtonScaleModel->func_clicked() = clan::bind_member(this, &MainWindow::on_menuScaleModelButton_down);
	pButtonIlluminatedModel->image_view()->set_image(clan::Image(canvas, "IlluminateOn.png", pSettings->fileResDoc.get_file_system()));
	pButtonIlluminatedModel->func_clicked() = clan::bind_member(this, &MainWindow::on_menuIlluminatedModelButton_down);

	// Отобразим окно настроек.
	pWindowSettings->initWindow(canvas);
	if (pSettings->getTopMenuIsSettingsWindowVisible()) {
		pMenuButton->set_pressed(true);
		on_menuButton_down();
	}

	// Настроим освещённость.
	if (pSettings->getTopMenuIsModelIlluminated()) {
		pButtonIlluminatedModel->set_pressed(true);
		on_menuIlluminatedModelButton_down();
	}

	// Отрисуем главное окно на текущем этапе.
	immediate_update();

	// Инициализируем модель.
	pWindowSettings->finishInit(wManager);
}


// A key was pressed
void MainWindow::on_input_down(const clan::KeyEvent &e)
{
	if (e.key() == clan::Key::escape)
	{
		on_window_close();
		dismiss();
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
void MainWindow::on_window_close()
{
	auto displayWindow = root_view()->view_tree()->display_window();
	auto pSettings = globalWorld.getSettingsStorage();

	// Определим состояние окна в формате clan::WindowShowType.
	clan::WindowShowType state = clan::WindowShowType::show_default;
	if (displayWindow.is_maximized())
		state = clan::WindowShowType::maximize;
	else if (displayWindow.is_minimized())
		state = clan::WindowShowType::minimize;

	// Сохраняем местоположение и состояние главного окна.
	pSettings->setMainWindowSettings(displayWindow.get_geometry(),
		state,
		displayWindow.is_fullscreen());
}

void MainWindow::on_mouse_down(const clan::InputEvent &key)
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

void MainWindow::on_menuButton_down()
{
	// Нажата ли кнопка отображения панели с настройками.
	bool isPressed = pMenuButton->pressed();

	// Показываем или прячем в клиентской области окно настроек.
	pWindowSettings->set_hidden(!isPressed);

	// Сохраняем настройку.
	auto pSettings = globalWorld.getSettingsStorage();
	pSettings->setTopMenuIsSettingsWindowVisible(isPressed);
}

void MainWindow::on_menuTopLeftModelButton_down()
{
	// Reset top left coordinate of the world.
	globalWorld.setAppearanceTopLeft(clan::Point());
}

void MainWindow::on_menuScaleModelButton_down()
{
	// Reset the scale.
	globalWorld.setAppearanceScale(1.0f);
}

void MainWindow::on_menuIlluminatedModelButton_down()
{
	// Показываем или прячем в клиентской области окно настроек.
	pModelRender->setIlluminatedWorld(pButtonIlluminatedModel->pressed());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	// Параметры окна.
	clan::DisplayWindowDescription desc;
	desc.set_main_window();
	desc.set_title(pSettings->LocaleStr(cMainWindowTitle));
	desc.set_allow_resize(true);
	desc.set_position(pSettings->getMainWindowPosition(), false);
	desc.set_visible(false);
	desc.set_fullscreen(pSettings->getIsFullScreen());

	// Create a window controller:
	pMainWindow = std::make_shared<MainWindow>();

	// To prevent auto sizing.
	pMainWindow->set_content_size(desc.get_size());

	// Показываем окно в состоянии, в котором оно было при закрытии (свёрнуто, максимизировано и т.д.)
	windowManager.present_main(pMainWindow, &desc, pSettings->getMainWindowState());

	// Доинициализируем окно для ресурсов, которым требуется оконная иерархия.
	pMainWindow->initWindow(&windowManager);

	// Инициализируем счётчик времени.
	game_time.reset();
}

App::~App()
{
}



bool App::update()
{
	auto& displayWindow = pMainWindow->root_view()->view_tree()->display_window();

	// Check for fullscreen switch.
	if (pMainWindow->fullscreen_requested != pMainWindow->is_fullscreen)
	{
		pMainWindow->is_fullscreen = pMainWindow->fullscreen_requested;
		displayWindow.toggle_fullscreen();
	}

	// Если окно свёрнуто, ничего не делаем.
	if (!displayWindow.is_minimized()) {

		// Выведем скорость обновления экрана и модели.
		int fps = int(game_time.get_updates_per_second());
		int tps = globalWorld.getUpdatesPerSecond();
		std::string fpsStr = clan::StringHelp::int_to_text(fps) + "/" + clan::StringHelp::int_to_text(tps);
		pMainWindow->pLabelFPS->set_text(fpsStr, true);

		// Выведем координаты левого верхнего угла мира.
		const clan::Point &topLeftWorld = globalWorld.getAppearanceTopLeft();
		if (pMainWindow->lastTopLeftWorld != topLeftWorld) {
			pMainWindow->lastTopLeftWorld = topLeftWorld;
			pMainWindow->pButtonTopLeftModelCoordinate->label()->set_text("X:Y "
				+ clan::StringHelp::int_to_text(int(topLeftWorld.x)) + ":"
				+ clan::StringHelp::int_to_text(int(topLeftWorld.y)), true);
		}

		// Выведем масштаб координат мира.
		float scaleWorld = globalWorld.getAppearanceScale();
		if (fabs(pMainWindow->lastScaleWorld - scaleWorld) > 0.0001f) {
			pMainWindow->lastScaleWorld = scaleWorld;
			pMainWindow->pButtonScaleModel->label()->set_text(pMainWindow->scaleLabelTemplate + clan::StringHelp::float_to_text(scaleWorld, 3, false), true);
		}

		// Проверим, не изменилась ли освещённость.
		bool illuminatedWorld = pMainWindow->pModelRender->getIlluminatedWorld();
		if (pMainWindow->lastIlluminatedWorld != illuminatedWorld) {
			pMainWindow->lastIlluminatedWorld = illuminatedWorld;
		}

		// Выведем время модели.
		const demi::DemiTime& modelTime = globalWorld.getModelTime();
		if (pMainWindow->lastModelTime != modelTime) {
			pMainWindow->lastModelTime = modelTime;
			pMainWindow->pLabelModelTime->set_text(modelTime.getDateStr(), true);
		}

		// Отрисовываем модель.
		pMainWindow->pModelRender->draw_without_layout();
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
	if (pMainWindow->pModelRender->getIsFrameUpdated())
		game_time.update();

	// Уведомляем окно настроек об отрисовке (для проверки наступления момента автосохранения модели).
	pMainWindow->pWindowSettings->modelRenderNotify(size_t(game_time.get_current_time()));

	// Выводим заэкранный буфер на экран.
	//displayWindow.flip();
	windowManager.flip();

	return true;
}

