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
	ModelRender();

	// Отрисовывает модель.
	void draw(clan::Canvas &canvas);

	// Включает или выключает подсетку.
	void toggleIlluminated() { illuminated = !illuminated; }

protected:
	/// Renders the content of a view
	virtual void render_content(clan::Canvas &canvas);

private:

	// Размер области для отрисовки модели (используется для ускорения, если размер не менялся).
	clan::Sizef oldWindowSize;

	// Буфер для отображения мира в виде точек при соответствующем масштабе.
	std::shared_ptr<clan::PixelBuffer> pPixelBuf;

	// Буфер для отрисовки по точкам, пересоздаётся при изменении размера, тут для оптимизации.
	std::shared_ptr<clan::Image> pImage;

	// Мировые координаты левого верхнего угла окна и масштаб.
	float xWorld = 0;
	float yWorld = 0;
	float scale = 1;

	// Истина, если включена подсветка.
	bool illuminated = false;

	// Истина, когда нажата средняя кнопка мыши и мы в режиме прокрутки.
	bool isScrollStart = false;

	// Точка нажатия кнопки мыши в оконных координатах и в мировых.
	float scrollWindowX, scrollWindowY, scrollWorldX, scrollWorldY;

	void on_mouse_down(clan::PointerEvent &e);
	void on_mouse_up(const clan::PointerEvent &e);
	void on_mouse_move(const clan::PointerEvent &e);
	void on_mouse_dblclk(const clan::PointerEvent &e);

	// Уменьшает масштаб - приближает поверхность.
	void Approach(const clan::Pointf &pos);

	// Увеличивает масштаб - отдаляет поверхность.
	void ToDistance(const clan::Pointf &pos);

	// Отрисовывает сетку.
	void DrawGrid(int width, int height);

	// Отрисовывает клетку в компактном виде - с координатой и ресурсами, которые поместятся.
	// В функцию передаётся точка поверхности, прямоугольник, где её необходимо отрисовать и мировые координаты точки относительно окна.
	void DrawCellCompact(const Dot &d, const clan::Rectf &rect, int x, int y);

};

