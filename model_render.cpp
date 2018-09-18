/* ===============================================================================
������������� �������� ������ ����.
��������� ��� ��������� ������.
05 june 2016.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "world.h"
#include "settings_storage.h"
#include "model_render.h"
#include <chrono>
#include <thread>

// �������, ��� ������� ���������� ������������ ����������� �� ������ �� ����������� ������.
const float cPixelDetailLevel = 0.1f;

// �������, ��� ������� ���������� ����������� ���������� � ���������� ����� ������� �������� � ������.
const float cCompactCellDetailLevel = 0.01f;

// �������, ��� ������� ���������� ����������� ���������� ������� � ������.
const float cAmountDetailLevel = 0.008f;

// ������ ����� ������ ��� ����������� �������� � ������ � ���������� �����.
const int cCompactCellResLineHeight = 21;

// ����������� ������ ������ � ���������� �����, ��� ������� ������� �����.
const int cCompactCellMinWidth = 30;

// ���������� �������� ��� ��������� ������� ����.
const float cScaleInc = 1.1f;

// ���������� ��������� ��� ���������.
const float xWorldInc = 120;
const float yWorldInc = xWorldInc;

// ��������� �������
const std::string cSolarTitle = "ModelRenderSolar";			// ������� ��� ��������� �������.
const std::string cEnergyTitle = "ModelRenderGeothermal";		// ������� ��� ������������� �������.


ModelRender::ModelRender(std::shared_ptr<SettingsStorage> &pSettingsStorage) : pSettings(pSettingsStorage),
	soundIlluminateOn("IlluminateOn.wav", false, pSettingsStorage->fileResDoc.get_file_system()),
	soundIlluminateOff("IlluminateOff.wav", false, pSettingsStorage->fileResDoc.get_file_system()),
	cellFont("Lucida Console", 12)
{
	slots.connect(sig_pointer_press(), this, &ModelRender::on_mouse_down);
	slots.connect(sig_pointer_release(), this, &ModelRender::on_mouse_up);
	slots.connect(sig_pointer_move(), this, &ModelRender::on_mouse_move);
	slots.connect(sig_pointer_double_click(), this, &ModelRender::on_mouse_dblclk);

	// �������� �����.
	thread = std::thread(&ModelRender::workerThread, this);

	// ������� ��� ��������� � ������������� �������.
	solarTitle = pSettings->LocaleStr(cSolarTitle);
	geothermalTitle = pSettings->LocaleStr(cEnergyTitle);
}

ModelRender::~ModelRender()
{
	// �������� ������ ������, ���� �� ���� ������, �� ��� �� ����� ����������� ���������.
	std::unique_lock<std::mutex> lock(threadMutex);
	threadExitFlag = true;
	lock.unlock();
	threadEvent.notify_all();
	thread.join();
}

void ModelRender::render_content(clan::Canvas &canvas)
{
	// ������������ ������ � ��������� �����.

	// ��. ����������.
	isFrameUpdated = false;

	// ������� ������� ��� �����������.
	const clan::Size windowSize(geometry().content_size());

	// ��� �����������.
	const clan::Size earthSize = globalWorld.get_worldSize();

	// ���� ������ ��� ������ ��������, �������. ��� windowSize �� ���� ���� ��������� �����������.
	if (earthSize.width == 0 || earthSize.height == 0)
		return;

	// ������������ ������� � ������� ����� ���� ������ �� ������ ��������� ��������.
	CorrectScale();

	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Point topLeftWorld = globalWorld.getAppearanceTopLeft();
	float scale = globalWorld.getAppearanceScale();

	// � ����������� �� ��������, ������������ ����� ���� ������.
	if (scale >= cPixelDetailLevel) {
		// ������ �������.

		// ���� ����� �����������, ������ ����� ���������.
		std::unique_lock<std::mutex> lock(threadMutex);
		if (!threadRunFlag) {

			// ���� ������ ���������, ������������ ������������.
			if (oldWindowSize != windowSize) {
				oldWindowSize = windowSize;
				pPixelBufToDraw = std::make_shared<clan::PixelBuffer>(windowSize.width, windowSize.height, clan::tf_rgba8);
				pPixelBufToWrite = std::make_shared<clan::PixelBuffer>(windowSize.width, windowSize.height, clan::tf_rgba8);
				pImage = std::make_shared<clan::Image>(canvas, *pPixelBufToDraw, clan::Rect(0, 0, windowSize));

				// �������� ����� � ������� ������ �������������.
				threadRunFlag = true;
				threadEvent.notify_all();
				do {
					lock.unlock();
					clan::System::sleep(100);
					lock.lock();
				} while (threadRunFlag);
			}

			// ������ ������ ������� ��� ���������� �������� ����������.
			std::shared_ptr<clan::PixelBuffer> tmp = pPixelBufToWrite;
			pPixelBufToWrite = pPixelBufToDraw;
			pPixelBufToDraw = tmp;

			// �������� �����.
			threadRunFlag = true;
			threadEvent.notify_all();
			isFrameUpdated = true;
		}

		// ������������ �������� �� �����.
		pImage->set_subimage(canvas, 0, 0, *pPixelBufToDraw, clan::Rect(0, 0, pPixelBufToDraw->get_size()));
		pImage->draw(canvas, 0, 0);
		// ��������� ������� ���������.
	}
	else {
		// ������ ��������.

		// ���������� �������������� ����� ����, �� ���� � ������ ������ � coordSystem.
		int xDotIndex = 0;
		int yDotIndex = 0;

		// ������� ������.
		clan::Rect r;

		// ���������� �� �����, ��� �����������.
		bool showTextInCell = scale < cCompactCellDetailLevel;

		// ��������� ������� ���������.
		LocalCoord coordSystem(topLeftWorld);

		// �������� �� ���������� ���������, ��� ��������.
		bool illuminated = getIlluminatedWorld();

		// ���� �����, ��� ����������� ���������� �������� ����.
		clan::Color color;
		Dot d;

		// � ����� ���������, ���� �� ������ �� ������� ����.
		//
		while (r.top < windowSize.height) {

			r.top = r.bottom;
			r.bottom = std::min(int((yDotIndex + 1) / scale + 0.5f), windowSize.height);

			// ���� �� �������� �����, �����������.
			if (r.get_height() < 1)
				break;

			r.left = r.right = 0;
			xDotIndex = 0;

			while (r.left < windowSize.width) {

				// ����� ����. 
				// ����������� �������� ������ ����� �����, � �� ������ - ����� ������� ��� ��������� ������ � ��������� ������.
				d = coordSystem.get_dot(xDotIndex, yDotIndex);

				// ������� ���� �����.
				d.get_color(color);

				// ���� �������� ���������, ������� �����-�����.
				if (illuminated)
					color.set_alpha(255);

				// ����� ��������� � ��������� ������, ��. �������� ++!.
				r.left = r.right;
				r.right = std::min(int((xDotIndex++ + 1) / scale + 0.5f), windowSize.width);

				if (r.get_width() < 1)
					break;

				// ������������ ������.
				canvas.fill_rect(r, clan::Colorf(color));

				// ������������ � ������������, ���� ����������.
				if (showTextInCell)
					DrawCellCompact(canvas, d, r, lround(topLeftWorld.x + xDotIndex - 1), lround(topLeftWorld.y + yDotIndex), color);
			}

			// ��������� � ���������� ���� ������.
			++yDotIndex;
		}

		// �������� �����.
		DrawGrid(canvas, windowSize);

		// ��������� ������ ���������.
		isFrameUpdated = true;
	}

}

void ModelRender::DrawGrid(clan::Canvas &canvas, const clan::Size &windowSize)
{
	// ������������ �����.

	// �������.
	float scale = globalWorld.getAppearanceScale();

	// ���������� ����� ������� ��� �������� � �������� ��������. ���������� �����, ������������ � ����.
	const int numLinesH = int(windowSize.height * scale + 0.5f);
	const int numLinesW = int(windowSize.width * scale + 0.5f);

	// ������������ �������������� � ������������ �����.
	for (int i = 1; i <= numLinesH; i++) {
		float y = i / scale;
		canvas.draw_line(0, y, float(windowSize.width), y, clan::Colorf::darkblue);
	}
	for (int i = 1; i <= numLinesW; i++) {
		float x = i / scale;
		canvas.draw_line(x, 0, x, float(windowSize.height), clan::Colorf::darkblue);
	}
}

void ModelRender::DrawCellCompact(clan::Canvas &canvas, const Dot &d, const clan::Rect &rect, int xLabel, int yLabel, const clan::Color& dotColor)
{
	// ������������ ������ � ���������� ���� - � ����������� � ���������, ������� ����������.

	// �������� �� ����������� ������.
	if ((rect.get_width() < cCompactCellMinWidth) || (rect.get_height() < cCompactCellResLineHeight))
		return;

	// ������, ��������� ��� ���������, �� ������� ������ ��� ���������� �����.
	const int h = rect.bottom - cCompactCellResLineHeight;

	// ������ ����� ��� ��������.
	const int indent = rect.left + 3;

	// ���������� ���������.
	const int elemCount = globalWorld.getElemCount();

	// ���������� ������� ������ ��� ������ ����������.
	int yLine = rect.top + 16;

	// ���� ������ ���� ����� ���� ������ � ����������� �� ����� ����� ������.
	clan::Colorf color((dotColor.get_red() + dotColor.get_green() + dotColor.get_blue()) * dotColor.get_alpha() < 255*1.5f ? clan::Colorf::white : clan::Colorf::black);

	// ������� ��������� �������.
	if (yLine < h) {
		cellFont.draw_text(canvas, float(indent), float(yLine), solarTitle + clan::StringHelp::float_to_text(d.getSolarEnergy() * 100) + "%", color);
		yLine += cCompactCellResLineHeight;

		// ������� ������������� �������.
		if (yLine < h) {
			cellFont.draw_text(canvas, float(indent), float(yLine), geothermalTitle + clan::StringHelp::float_to_text(d.getGeothermalEnergy() * 100) + "%", color);
			yLine += cCompactCellResLineHeight;

			// ������������ ������ � ���������, ������� ����������.
			int i = 0;
			while (yLine < h) {

				// ���� ������ �� ������� ��� �����������, ���������� ���.
				if (globalWorld.getResVisibility(i)) {

					// �������� ����� � ����������� ������� ��� ����������� ��������������, ��� ��� ��������� ����� \t �� ��������.
					//
					std::string str = globalWorld.getResName(i);
					size_t len = str.length();
					size_t tabbed = std::max<size_t>(20, (size_t(0.1f * len) + 1) * 10);
					if (len < tabbed)
						str += std::string(tabbed - len, ' ');
					//str +=  + "%\t" + ;

					// ���� �������� ���, ������ ������� � ��.
					const unsigned long long amnt = d.getElemAmount(i);
					if (amnt) {
						
						// �������� � ���������.
						std::string strPers = clan::StringHelp::float_to_text(d.getElemAmountPercent(i), 2) + "% ";
						len = strPers.length();
						if (len < 8)
							strPers += std::string(8 - len, ' ');

						// ��������� ������ � ���������� � ���������� ����������.
						str += ": " + strPers + IntToStrWithDigitPlaces(amnt);

					} else
						str += ": ---";


					// ������� ������.
					cellFont.draw_text(canvas, float(indent), float(yLine), str, color);
					yLine += cCompactCellResLineHeight;
				}

				// ��������� � ���������� ��������, ���� ����.
				if (++i >= elemCount)
					break;
			}

			// ���������� ������ ������, ��� ���������.
			int detrit = 0;

			// ������������ ��������� ����� ������.
			for (auto &cell : d.cells) {

				// ���� ��������� �����, �����������.
				if (yLine > h)
					break;

				// ��������, �������� ����������� ������.
				demi::Organism *organism = cell->organism;
				if (organism != nullptr) {
					// ��������, �������� �� ����������� ��� ������� ����.
					if (organism->get_species()->get_visible()) {
						cellFont.draw_text(canvas, float(indent), float(yLine), organism->get_species()->name + '\t' + clan::StringHelp::float_to_text(organism->getVitality()) + "", color);

						yLine += cCompactCellResLineHeight;
					}
				}
				else
					detrit++;

			}

			// ������������ ���������� ������� ������, ���� ���� �����.
			if (yLine < h && detrit > 0) {
				cellFont.draw_text(canvas, float(indent), float(yLine), "Detrit\t"  + clan::StringHelp::int_to_text(detrit) + ".", color);

				yLine += cCompactCellResLineHeight;
			}
		}
	}

	// ������������ � ������ ����� ���� ����������.
	// ������� ������������ �� ���������, ��� ��� �����������, ��� ��� ������ ����, ������� ���� ���������.
	cellFont.draw_text(canvas, float(indent), float(rect.bottom - 3), "X:Y " + clan::StringHelp::int_to_text(xLabel) + ":" + clan::StringHelp::int_to_text(yLabel), color);
}


inline int max1(float a)
{
	return int(a < 1 ? 1 : a);
}

void ModelRender::on_mouse_down(clan::PointerEvent &e)
{
	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Point topLeftWorld = globalWorld.getAppearanceTopLeft();
	float scale = globalWorld.getAppearanceScale();

	switch (e.button())
	{
	case clan::PointerButton::middle:

		// �������� ���� ��������� � ���������� �������
		isScrollStart = true;
		scrollWindow = clan::Point(e.pos(this));
		scrollWorld = topLeftWorld;
		break;

	case clan::PointerButton::wheel_up:

		// ���� ������ ������� Ctrl, ��������� �����, ���� Shift, ��������� ������, ����� �����������.
		if (e.shift_down())
			topLeftWorld.x -= max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			topLeftWorld.y -= max1(yWorldInc * scale);
		else
			DoScaleStep(e.pos(this), 1/cScaleInc, topLeftWorld);

		// �������� ���������.
		globalWorld.setAppearanceTopLeft(topLeftWorld);

		// ������������ ������� � ������� ����� ���� ������ ��-��������� ������ �� �������.
		CorrectScale();
		break;

	case clan::PointerButton::wheel_down:
		if (e.shift_down())
			topLeftWorld.x += max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			topLeftWorld.y += max1(yWorldInc * scale);
		else
			DoScaleStep(e.pos(this), cScaleInc, topLeftWorld);
		globalWorld.setAppearanceTopLeft(topLeftWorld);
		CorrectScale();
		break;
	}
}

void ModelRender::on_mouse_up(const clan::PointerEvent &e)
{
	switch (e.button())
	{
	case clan::PointerButton::middle:
		isScrollStart = false;
		break;
	}
}

void ModelRender::on_mouse_move(const clan::PointerEvent &e)
{
	if (isScrollStart) {

		// �������� ���� ������������ ����� ������� ������� ������.
		clan::Pointf dif = scrollWindow - clan::Point(e.pos(this));

		// �� ������������ �������� ����� ����� ������� ���������� � ������ ��������.
		globalWorld.setAppearanceTopLeft(scrollWorld + clan::Point(dif * globalWorld.getAppearanceScale()));

		// ������������ ������� � ������� ����� ���� ������ ��-��������� ������ �� �������.
		CorrectScale();
	}
}

void ModelRender::on_mouse_dblclk(const clan::PointerEvent &e)
{
	switch (e.button())
	{
	case clan::PointerButton::middle:

		// ���� ������� ������ � ���������, ���� ����������� ����������, ����� ����������� ������������.
		float scaleStep = 0;

		// ������ ��������� ����� �� �������� �������� �� ����������.
		//
		if (globalWorld.getAppearanceScale() <= cCompactCellDetailLevel) {

			// ������� ���� ��� �����������.
			const clan::Sizef windowSize = geometry().content_size();

			// ������� ���� (��� ��������).
			const clan::Sizef worldSize(globalWorld.get_worldSize());

			// ��������� �����������-��������� �������.
			const float maxScale = std::min<float>(worldSize.width / windowSize.width, worldSize.height / windowSize.height);

			// ������������ �� ��������� ��� �� 1.
			scaleStep = float(pow(globalWorld.getAppearanceScale() / (maxScale < 1 ? maxScale : 1), 1.0 / 13));
		}
		else
			scaleStep = float(pow(globalWorld.getAppearanceScale() / cCompactCellDetailLevel, 1.0 / 12));


		// ��������� ��������.
		//
		clan::Point topLeftWorld = globalWorld.getAppearanceTopLeft();
		for (int i = 0; i <= 12; ++i) {
			DoScaleStep(e.pos(this), 1/scaleStep, topLeftWorld);
			globalWorld.setAppearanceTopLeft(topLeftWorld);
			
			// ������ ��������.
			draw_without_layout();
			view_tree()->display_window().flip();
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}

		// ���� ��� ���� ������������ ���������, ������� �� �������� �������, ������ ��� �����-�� ������ ����������.
		if (scaleStep < 1)
			globalWorld.setAppearanceTopLeft(clan::Point());

		break;
	}
}


void ModelRender::DoScaleStep(const clan::Pointf &pos, float scaleStep, clan::Point &newTopLeft)
{
	// �������� ������� �� 1 ��� - �������� ��� ���������� �����������.

	// �������.
	float scale = globalWorld.getAppearanceScale();

	// ������ �������.
	scale *= scaleStep;

	// ���� ������� �� �����������, �� �������, ��� ������� ��������� ����������, ������ �������� ��� ��������. ��� �����
	// ���������� �������������� ������������� ���������, �� ���� ���������� ���������� �����������
	// ��� �������� �� ��������� ��������, �������� �������, ���������� ����� ���������� �����������
	// � �� ��� ������ ������� ���������. ��� ��������� ������ ���������� ����� ��� ��������.
	const clan::Sizef viewSize = geometry().content_size() * scale;
	const clan::Sizef worldSize = globalWorld.get_worldSize();

	// ��������� �������� ������ ���� �� �������� ������������ ��������.
	if (viewSize.width <= worldSize.width && viewSize.height <= worldSize.height) {
		float wx1 = pos.x * scale / scaleStep;
		float wy1 = pos.y * scale / scaleStep;
		float wx2 = pos.x * scale;
		float wy2 = pos.y * scale;
		// �������� ���������.
		newTopLeft.x += int(wx1 - wx2 + 0.5f);
		newTopLeft.y += int(wy1 - wy2 + 0.5f);
	}

	// �������� ���������.
	globalWorld.setAppearanceScale(scale);
}

// ���������� ��������� ����.
void ModelRender::setIlluminatedWorld(bool newValue) 
{ 
	pSettings->setTopMenuIsModelIlluminated(newValue);
	if (newValue)
		soundIlluminateOn.play();
	else
		soundIlluminateOff.play();
}

void ModelRender::workerThread()
{
	// ������� ������� ������, ������������ ������������.

	// ��� ����������� �������� ����.
	Dot d;

	try
	{
		while (true)
		{
			// ��������������� �� ��������� ������, �������� ��� ������������� �������� �����.
			std::unique_lock<std::mutex> lock(threadMutex);
			threadEvent.wait(lock, [&]() { return threadRunFlag || threadExitFlag; });

			// ���� ���� ������� �� �����, ��������� ������.
			if (threadExitFlag)
				break;

			// ������� ������� ��� �����������.
			const clan::Sizef windowSize = geometry().content_size();
			float width = windowSize.width;
			float height = windowSize.height;
			float scale = globalWorld.getAppearanceScale();

			// ��������� ������� ���������.
			LocalCoord coordSystem(globalWorld.getAppearanceTopLeft());

			// �������� �� ���������� ���������, ��� ��������.
			bool illuminated = getIlluminatedWorld();

			// ������������ �������� �����.
			lock.unlock();

			// ��������� ������ �������������.

			// ��������� �� ����� ������.
			unsigned char *pixels = (unsigned char *)pPixelBufToWrite->get_data();

			// ���������� ���� ��� ���� ������ � ������.
			const int lineSize = int(width * 4);

			// ���� �����, ��� ����������� ���������� �������� ����.
			clan::Color color;

			// ������ ������� �����.
			int xIndex, oldXIndex = -1, yIndex, oldYIndex = -1;

			for (int ypos = 0; ypos < height; ++ypos)
			{
				// ������ ����� ����.
				yIndex = int(ypos * scale + 0.5f);

				// ���� ������ �� ���������, ����� ������ ����������� ���������� ������, ����� ��������� ������.
				if (oldYIndex == yIndex) {
					memcpy(pixels, pixels - lineSize, lineSize);
					pixels+=lineSize;
				}
				else {
					oldXIndex = -1;
					oldYIndex = yIndex;

					for (int xpos = 0; xpos < width; ++xpos)
					{
						// ����� ����. ������ ����� ������ ������, ��� � ���������� ������� ����� ����� ���� ����������� �����, ���� ��� ����� ��� �����������.
						xIndex = int(xpos * scale + 0.5f);

						// ���� ���� ���������� �� ��������, �� ������ ��������� ����������.
						if (xIndex != oldXIndex) {
							d = coordSystem.get_dot(xIndex, yIndex);
							d.get_color(color);
							oldXIndex = xIndex;
						}

						*(pixels++) = color.get_red();
						*(pixels++) = color.get_green();
						*(pixels++) = color.get_blue();
						*(pixels++) = illuminated ? 255 : color.get_alpha();
					}
				}
			}

			// ��������� �������� ����� ��� ���������� ����������.
			lock.lock();

			// �������� � ������� ����������, ��� ���� �����������.
			threadRunFlag = false;

			// ��� �������� ����� �������������� ��� ���������� ����� {}
		}
	}
	catch (clan::Exception &)
	{
		// ������� �� ��������� ���������� ������. ����� ������ ������� �����.
		std::unique_lock<std::mutex> lock(threadMutex);
		threadCrashedFlag = true;
	}
}

// ������������ ������� � ������� ����� ���� ������ ����� ��������� ��������.
void ModelRender::CorrectScale()
{
	// ������� ������� ��� �����������.
	const clan::Sizef windowSize = geometry().content_size();

	// ������ ����.
	const clan::Size earthSize = globalWorld.get_worldSize();

	// ���� ������ ��� ������ ��������, �������.
	if (windowSize.width == 0 || windowSize.height == 0 || earthSize.width == 0 || earthSize.height == 0)
		return;

	// ������ � ������� �����������.
	const clan::Sizef scaledSize(windowSize * globalWorld.getAppearanceScale());

	// ���� ������ ���� ���� ������ ������������� ����, ���� ���������������� �������.
	if (scaledSize.width > earthSize.width || scaledSize.height > earthSize.height) {
		float w = earthSize.width / windowSize.width;
		float h = earthSize.height / windowSize.height;
		globalWorld.setAppearanceScale(w < h ? w : h);
	}
	else {
		// ������� ���������� ������ �������� ���� ����.
		clan::Point topLeftWorld = globalWorld.getAppearanceTopLeft();

		// ���� � ���������� ���������� ������� ���� �������� �� ������� ����, ���� ���������������� ������� ����������.
		// �� ��������� ������������, �� ����������� - ����������� ���������.
		if (topLeftWorld.x < 0)
			topLeftWorld.x += earthSize.width;
		else if (topLeftWorld.x >= earthSize.width)
			topLeftWorld.x -= earthSize.width;

		if (topLeftWorld.y < 0)
			topLeftWorld.y = 0;
		else if (topLeftWorld.y + scaledSize.height > earthSize.height)
			topLeftWorld.y = int(earthSize.height - scaledSize.height);
		
		// ��������� ���������.
		globalWorld.setAppearanceTopLeft(topLeftWorld);
	}
}
