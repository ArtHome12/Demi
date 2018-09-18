/* ===============================================================================
Моделирование эволюции живого мира.
Процедуры для отрисовки модели.
05 june 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

class ModelRender: public clan::View
{
public:
	ModelRender(std::shared_ptr<SettingsStorage> &pSettingsStorage);
	~ModelRender();

	// Постоянная подсветка мира.
	const bool getIlluminatedWorld() { return pSettings->getTopMenuIsModelIlluminated(); }
	void setIlluminatedWorld(bool newValue);

	const bool getIsFrameUpdated() { return isFrameUpdated; }

protected:
	/// Renders the content of a view
	void render_content(clan::Canvas &canvas) override;

private:
	// Настройки.
	std::shared_ptr<SettingsStorage> &pSettings;

	// Размер области для отрисовки модели (используется для ускорения, если размер не менялся).
	clan::Size oldWindowSize;

	// Буфер для отображения мира в виде точек при соответствующем масштабе.
	std::shared_ptr<clan::PixelBuffer> pPixelBufToWrite;
	std::shared_ptr<clan::PixelBuffer> pPixelBufToDraw;

	// Буфер для отрисовки по точкам, пересоздаётся при изменении размера, тут для оптимизации.
	std::shared_ptr<clan::Image> pImage;

	// Шрифт для отрисовки надписей в клетке.
	clan::Font cellFont;

	// Истина, когда нажата средняя кнопка мыши и мы в режиме прокрутки.
	bool isScrollStart = false;

	// Точка нажатия кнопки мыши в оконных координатах и в мировых.
	clan::Point scrollWindow;
	clan::Point scrollWorld;

	// Подписи для солнечной и геотермальной энергий.
	std::string solarTitle;
	std::string geothermalTitle;

	// Флаг, предназначенный для корректного отображения fps в App::update() в случае, если картинка не обновилась, а выдаём старый буфер.
	// Обновляется в render_content().
	bool isFrameUpdated = false;

	void on_mouse_down(clan::PointerEvent &e);
	void on_mouse_up(const clan::PointerEvent &e);
	void on_mouse_move(const clan::PointerEvent &e);
	void on_mouse_dblclk(const clan::PointerEvent &e);

	// Увеличивает масштаб - отдаляет поверхность.
	void DoScaleStep(const clan::Pointf &pos, float scaleStep, clan::Point &newTopLeft);

	// Отрисовывает сетку.
	void DrawGrid(clan::Canvas &canvas, const clan::Size &windowSize);

	// Отрисовывает клетку в компактном виде - с координатой и ресурсами, которые поместятся.
	void DrawCellCompact(clan::Canvas &canvas, const Dot &d, const clan::Rect &rect, int xLabel, int yLabel, const clan::Color& dotColor);

	// Звуки включения и выключения подсветки.
	clan::SoundBuffer soundIlluminateOn;
	clan::SoundBuffer soundIlluminateOff;

	// Поток, создающий пиксельбуфер.
	std::thread thread;

	// Ширина 1 символа текстом клетки.
	int cellFontSymbolWidth = 0;

	// Флаги потока, создающего пиксельбуфер.
	bool threadExitFlag = false;		// Если истина, поток должен завершиться.
	bool threadRunFlag = false;		// Если истина, то поток работает и изменяет модель, иначе простаивает.
	bool threadCrashedFlag = false;	// Если истина, значит поток завершился аварийно.
	std::mutex threadMutex;
	std::condition_variable threadEvent;

	// Рабочая функция потока, вычисляющего модель.
	void workerThread();

	// Корректирует масштаб и верхний левый угол модели после изменения размеров.
	void CorrectScale();
};

