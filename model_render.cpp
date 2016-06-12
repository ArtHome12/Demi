/* ===============================================================================
Моделирование эволюции живого мира.
Процедуры для отрисовки модели.
05 june 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "earth.h"
#include "settings_storage.h"
#include "model_render.h"

// Масштаб, при котором происходит переключение отображения по точкам на отображение клеток.
const float cPixelDetailLevel = 0.15f;

// Масштаб, при котором включается отображение линий клеток.
const float cLinesDetailLevel = 0.1f;

// Масштаб, при котором включается отображение координаты и компактной формы наличия ресурсов в клетке.
const float cCompactCellDetailLevel = 0.01f;

// Масштаб, при котором включается отображение количества веществ в клетке.
const float cAmountDetailLevel = 0.008f;

// Высота одной строки для отображения ресурсов в клетке в компактной форме.
const int cCompactCellResLineHeight = 30;

// Приращение масштаба при прокрутке колёсика мыши.
const float cScaleInc = 1.1f;

// Приращение координат при прокрутке.
const float xWorldInc = 120;
const float yWorldInc = xWorldInc;

const std::string cSolar = "Solar";			// Подпись для солнечной энергии.
const std::string cEnergy = "Geothermal";	// Подпись для геотермальной энергии.


ModelRender::ModelRender(std::shared_ptr<SettingsStorage> &pSettingsStorage) : pSettings(pSettingsStorage),
	soundIlluminateOn("IlluminateOn.wav", false, pSettingsStorage->fileResDoc.get_file_system()),
	soundIlluminateOff("IlluminateOff.wav", false, pSettingsStorage->fileResDoc.get_file_system()),
	cellFont("Tahoma", 12)
{
	slots.connect(sig_pointer_press(), this, &ModelRender::on_mouse_down);
	slots.connect(sig_pointer_release(), this, &ModelRender::on_mouse_up);
	slots.connect(sig_pointer_move(), this, &ModelRender::on_mouse_move);
	slots.connect(sig_pointer_double_click(), this, &ModelRender::on_mouse_dblclk);
}


void ModelRender::render_content(clan::Canvas &canvas)
{
	// Отрисовывает модель в указанном месте.
	//
	// Получим размеры для отображения.
	const clan::Sizef windowSize = geometry().content_size();

	// Для оптимизации.
	const clan::Sizef earthSize = globalEarth.get_worldSize();

	// Если некуда или нечего рисовать, выходим.
	if (windowSize.width == 0 || windowSize.height == 0 || earthSize.width == 0 || earthSize.height == 0)
		return;

	// Если размер поменялся, пересоздадим пиксельбуфер.
	if (oldWindowSize != windowSize) {
		oldWindowSize = windowSize;
		pPixelBuf = std::make_shared<clan::PixelBuffer> (int(windowSize.width), int(windowSize.height), clan::tf_rgba8);
		pImage = std::make_shared<clan::Image>(canvas, *pPixelBuf, clan::Rectf(0, 0, windowSize));
	}

	// Для удобства, размер в мировых координатах.
	const float scaledWidth = windowSize.width * scale;
	const float scaledHeight = windowSize.height * scale;

	// Если размер окна стал больше отображаемого мира, надо откорректировать масштаб.
	if (scaledWidth > earthSize.width || scaledHeight > earthSize.height) {
		float w = earthSize.width / windowSize.width;
		float h = earthSize.height / windowSize.height;
		scale = w < h ? w : h;
	}
	else {

		// Если в результате увеличения размера окно выезжает за границу мира, надо откорректировать мировые координаты.
		// По вертикали ограничиваем, по горизонтали - циклическая прокрутка.
		if (topLeftWorld.x < 0.0f)
			topLeftWorld.x += earthSize.width;
		else if (topLeftWorld.x >= earthSize.width)
			topLeftWorld.x -= earthSize.width;

		if (topLeftWorld.y < 0.0f)
			topLeftWorld.y = 0.0f;
		else if (topLeftWorld.y + scaledHeight > earthSize.height)
			topLeftWorld.y = earthSize.height - scaledHeight;
	}

	// Определим систему координат.
	LocalCoord coordSystem(globalEarth.getCopyDotsArray(), topLeftWorld);

	// Включена ли постоянная подсветка, для удобства.
	bool illuminated = getIlluminatedWorld();

	// Цвет точки, для оптимизации объявление вынесено сюда.
	clan::Colorf color;

	// В зависимости от масштаба, отрисовываем точки либо клетки.
	if (scale >= cPixelDetailLevel) {
		// Рисуем точками.

		// Указатель на точки буфера.
		pPixelBuf->lock(canvas, clan::access_write_only);
		unsigned char *pixels = (unsigned char *)pPixelBuf->get_data();

		for (int ypos = 0; ypos < windowSize.height; ypos++)
		{
			for (int xpos = 0; xpos < windowSize.width; xpos++)
			{
				// Точка мира. Доступ через индекс потому, что в физической матрице точки могут быть расположены иначе, хотя это повод для оптимизации.
				const Dot &d = coordSystem.get_dot(xpos * scale, ypos * scale);

				d.get_color(color);

				*(pixels++) = (unsigned char)(color.get_red() * 255);
				*(pixels++) = (unsigned char)(color.get_green() * 255);
				*(pixels++) = (unsigned char)(color.get_blue() * 255);
				*(pixels++) = illuminated ? 255 : (unsigned char)(color.get_alpha() * 255);
			}
		}

		// Отрисовываем картинку на канве. Пиксельбуфер уже связан с картинкой при создании её выше.
		pPixelBuf->unlock();
		pImage->set_subimage(canvas, 0, 0, *pPixelBuf, clan::Rect(0, 0, pPixelBuf->get_size()));
		pImage->draw(canvas, 0, 0);
		// Рисование точками завершено.
	}
	else {
		// Рисуем клетками.

		// Координаты отрисовываемой точки мира, от нуля с учётом сдвига в coordSystem.
		float xDotIndex = 0;
		float yDotIndex = 0;

		// Область клетки.
		clan::Rectf r;

		// Помещается ли текст, для оптимизации.
		bool showTextInCell = scale < cCompactCellDetailLevel;

		// В цикле двигаемся, пока не выйдем за границу окна.
		//
		while (r.top < windowSize.height) {

			r.top = r.bottom;
			r.bottom = (yDotIndex + 1) / scale;

			r.left = r.right = 0;
			xDotIndex = 0;

			while (r.left < windowSize.width) {

				// Точка мира. 
				const Dot &d = coordSystem.get_dot(xDotIndex, yDotIndex);

				// Получим цвет точки.
				d.get_color(color);

				// Если включено освещение, заменим альфа-канал.
				if (illuminated)
					color.set_alpha(1);

				// Сразу переходим к следующей клетке, см. оператор ++!.
				r.left = r.right;
				r.right = (xDotIndex++ + 1) / scale;

				// Отрисовываем клетку.
				canvas.fill_rect(r, color);

				// Отрисовываем её координату, если помещается.
				if (showTextInCell)
					DrawCellCompact(canvas, d, r, int(topLeftWorld.x + xDotIndex - 1), int(topLeftWorld.y + yDotIndex));
			}

			// Переходим к следующему ряду клеток.
			++yDotIndex;
		}

		// Отрисуем линии, если они помещаются.
		//
		if (scale < cLinesDetailLevel)
			DrawGrid(canvas, windowSize);

		// Рисование клеток завершено.
	}

}

void ModelRender::DrawGrid(clan::Canvas &canvas, const clan::Sizef &windowSize)
{
	// Отрисовывает сетку.

	// Расстояние между линиями это обратная к масштабу величина. Количество линий, помещающихся в окне.
	//
	float numHLines = roundf(windowSize.height * scale);
	float numVLines = roundf(windowSize.width * scale);

	// Отрисовываем горизонтальные и вертикальные линии.
	for (float i = 0; i <= numHLines; i++) {
		float y = i / scale;
		canvas.draw_line(0, y, windowSize.width, y, clan::Colorf::darkblue);
	}
	for (float i = 0; i <= numVLines; i++) {
		float x = i / scale;
		canvas.draw_line(x, 0, x, windowSize.height, clan::Colorf::darkblue);
	}
}

// Отрисовывает клетку в компактном виде - с координатой и ресурсами, которые поместятся.
// В функцию передаётся точка поверхности, прямоугольник, где её необходимо отрисовать и мировые координаты точки относительно окна.
//
void ModelRender::DrawCellCompact(clan::Canvas &canvas, const Dot &d, const clan::Rectf &rect, int xLabel, int yLabel)
{
	// Ограничиваем область отрисовки
	//canvas.set_cliprect(rect);

	// Кличество строк, которые влезли бы в указанное пространство, исключая место под координату.
	float numLinesInRect = roundf(rect.get_height() / cCompactCellResLineHeight - 1);

	// Количество строк с ресурсами - все, включая энергии, или сколько поместится.
	float numLines = clan::min(float(globalEarth.getElemCount() + 2), numLinesInRect);

	// Отступ слева для надписей.
	const float indent = rect.left + 3;

	// Отрисовываем солнечную энергию, геотермальную и количества элементов.
	if (numLines > 0) {

		// Координата строки.
		float yLine = rect.top + 16;

		cellFont.draw_text(canvas, indent, yLine, cSolar + '\t' + clan::StringHelp::float_to_text(d.solarEnergy * 100) + "%");

		if (numLines > 1) {

			yLine += cCompactCellResLineHeight;
			cellFont.draw_text(canvas, indent, yLine, cEnergy + '\t' + clan::StringHelp::float_to_text(d.energy * 100) + "%");
			yLine += cCompactCellResLineHeight;


			// В цикле отрисовываем строки с ресурсами.
			for (int i = 0; i < numLines - 2; i++) {

				// Значение в процентах.
				//float percent = d.res[i];
				float percent = d.res[i] * 100 / globalEarth.getResMaxValue(i);

				cellFont.draw_text(canvas, indent, yLine, globalEarth.getResName(i) + '\t' + clan::StringHelp::float_to_text(percent) + "%");
				yLine += cCompactCellResLineHeight;
			}
		}
	}

	// Отрисовываем в нижнем левом углу координату.
	cellFont.draw_text(canvas, indent, rect.bottom - 3, "X:Y " + clan::StringHelp::int_to_text(xLabel) + ":" + clan::StringHelp::int_to_text(yLabel));

	// Восстановим область отрисовки на всё полотно.
	//canvas.reset_cliprect();
}

// Отрисовывает модель.
void ModelRender::draw(clan::Canvas &canvas)
{
	// For convenience and optimization get refs
	const clan::ViewGeometry &geom = geometry();

	// Очистим канву.
	canvas.fill_rect(geom.content_box(), clan::Colorf::black);

	// Prepare the canvas for render_content()
	clan::Pointf translate = geom.content_pos();
	clan::TransformState transform_state(&canvas);
	canvas.set_transform(transform_state.matrix * clan::Mat4f::translate(translate.x, translate.y, 0) * view_transform());
	
	// Draw
	render_content(canvas);
}

int max1(float a)
{
	return int(a < 1 ? 1 : a);
}

void ModelRender::on_mouse_down(clan::PointerEvent &e)
{
	switch (e.button())
	{
	case clan::PointerButton::middle:

		// Включаем флаг прокрутки и запоминаем позицию
		//
		isScrollStart = true;
		scrollWindow = e.pos(this);
		scrollWorld = topLeftWorld;

		// Захватываем события мыши, чтобы узнать об отпускании кнопки.
		//pMainWindow->capture_mouse(true);

		break;

	case clan::PointerButton::wheel_up:

		// Если зажата клавиша Ctrl, прокрутка вверх, если Shift, прокрутка вправо, иначе приближение.
		//
		if (e.shift_down())
			topLeftWorld.x -= max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			topLeftWorld.y -= max1(yWorldInc * scale);
		else
			Approach(e.pos(this), cScaleInc);
		break;

	case clan::PointerButton::wheel_down:
		if (e.shift_down())
			topLeftWorld.x += max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			topLeftWorld.y += max1(yWorldInc * scale);
		else
			ToDistance(e.pos(this), cScaleInc);
		break;
	}
}

void ModelRender::on_mouse_up(const clan::PointerEvent &e)
{
	switch (e.button())
	{
	case clan::PointerButton::middle:
		isScrollStart = false;
		//pMainWindow->capture_mouse(false);
		break;
	}
}

void ModelRender::on_mouse_move(const clan::PointerEvent &e)
{
	if (isScrollStart) {

		// Смещение мыши относительно точки нажатия средней кнопки.
		clan::Pointf dif = scrollWindow - e.pos(this);

		// По определённому смещению задаём новую мировую координату с учётом масштаба.
		topLeftWorld = scrollWorld + dif * scale;
	}
}

void ModelRender::on_mouse_dblclk(const clan::PointerEvent &e)
{
	switch (e.button())
	{
	case clan::PointerButton::middle:

		// Если масштаб клетки с надписями, надо максимально отдалиться, иначе максимально приблизиться.
		float scaleStep = 0;

		// Делаем несколько шагов от текущего масштаба до требуемого.
		//
		if (scale <= cCompactCellDetailLevel) {

			// Размеры окна для отображения.
			const clan::Sizef windowSize = geometry().content_size();

			// Размеры мира (для удобства).
			const clan::Sizef worldSize = globalEarth.get_worldSize();

			// Определим максимально-возможный масштаб.
			const float maxScale = std::min<float>(worldSize.width / windowSize.width, worldSize.height / windowSize.height);

			// Масштабируем до максимума или до 1.
			scaleStep = float(pow(scale / (maxScale < 1 ? maxScale : 1), 1.0 / 13));
		}
		else
			scaleStep = float(pow(scale / cCompactCellDetailLevel, 1.0 / 12));


		// Запускаем анимацию.
		//
		for (int i = 0; i <= 12; ++i) {
			Approach(e.pos(this), scaleStep);
			//render_content();
		}

		break;
	}
}

void ModelRender::Approach(const clan::Pointf &pos, float scaleStep)
{
	// Уменьшает масштаб - приближает поверхность.
	//
	// Область, над которой произошло увеличение, должна остаться под курсором. Для этого
	// необходимо компенсировать нежелательную прокрутку, то есть определить координату поверхности
	// под курсором до изменения масштаба, изменить масштаб, определить новую координату поверхности
	// и на эту дельту сделать прокрутку.
	//

	// Координаты мира под курсором до масштабирования (без учёта прокрутки topLeftWorld.x для оптимизации).
	float wx1 = pos.x * scale;
	float wy1 = pos.y * scale;

	// Уменьшаем масштаб или увеличиваем (приближаем) поверхность.
	scale /= scaleStep;

	// Координаты мира под курсором после масштабирования.
	//
	float wx2 = pos.x * scale;
	float wy2 = pos.y * scale;

	topLeftWorld.x += int(wx1 - wx2);
	topLeftWorld.y += int(wy1 - wy2);
}


void ModelRender::ToDistance(const clan::Pointf &pos, float scaleStep)
{
	// Увеличивает масштаб - отдаляет поверхность.
	//
	// Меняем масштаб, отодвигая поверхность.
	scale *= scaleStep;

	//const clan::Rect &viewRect = geometry().content_box();
	const clan::Sizef viewSize = geometry().content_size() * scale;
	const clan::Sizef worldSize = globalEarth.get_worldSize();

	// Коррекцию проводим только если не достигли минимального масштаба.
	if (viewSize.width <= worldSize.width && viewSize.height <= worldSize.height) {
		float wx1 = pos.x * scale / scaleStep;
		float wy1 = pos.y * scale / scaleStep;
		float wx2 = pos.x * scale;
		float wy2 = pos.y * scale;
		topLeftWorld.x += int(wx1 - wx2);
		topLeftWorld.y += int(wy1 - wy2);
	}
}

// Постоянная подсветка мира.
void ModelRender::setIlluminatedWorld(bool newValue) 
{ 
	pSettings->setTopMenuIsModelIlluminated(newValue);
	if (newValue)
		soundIlluminateOn.play();
	else
		soundIlluminateOff.play();
}
