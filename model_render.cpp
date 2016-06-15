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

// �������, ��� ������� ���������� ������������ ����������� �� ������ �� ����������� ������.
const float cPixelDetailLevel = 0.1f;

// �������, ��� ������� ���������� ����������� ���������� � ���������� ����� ������� �������� � ������.
const float cCompactCellDetailLevel = 0.01f;

// �������, ��� ������� ���������� ����������� ���������� ������� � ������.
const float cAmountDetailLevel = 0.008f;

// ������ ����� ������ ��� ����������� �������� � ������ � ���������� �����.
const int cCompactCellResLineHeight = 30;

// ���������� �������� ��� ��������� ������� ����.
const float cScaleInc = 1.1f;

// ���������� ��������� ��� ���������.
const float xWorldInc = 120;
const float yWorldInc = xWorldInc;

const std::string cSolar = "Solar";			// ������� ��� ��������� �������.
const std::string cEnergy = "Geothermal";	// ������� ��� ������������� �������.


ModelRender::ModelRender(std::shared_ptr<SettingsStorage> &pSettingsStorage) : pSettings(pSettingsStorage),
	soundIlluminateOn("IlluminateOn.wav", false, pSettingsStorage->fileResDoc.get_file_system()),
	soundIlluminateOff("IlluminateOff.wav", false, pSettingsStorage->fileResDoc.get_file_system()),
	cellFont("Tahoma", 12)
{
	slots.connect(sig_pointer_press(), this, &ModelRender::on_mouse_down);
	slots.connect(sig_pointer_release(), this, &ModelRender::on_mouse_up);
	slots.connect(sig_pointer_move(), this, &ModelRender::on_mouse_move);
	slots.connect(sig_pointer_double_click(), this, &ModelRender::on_mouse_dblclk);

	// �������� �����.
	thread = std::thread(&ModelRender::workerThread, this);
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

	// ������� ������� ��� �����������.
	const clan::Sizef windowSize = geometry().content_size();

	// ��� �����������.
	const clan::Sizef earthSize = globalWorld.get_worldSize();

	// ���� ������ ��� ������ ��������, �������.
	if (windowSize.width == 0 || windowSize.height == 0 || earthSize.width == 0 || earthSize.height == 0)
		return;

	// ������������ ������� � ������� ����� ���� ������ �� ������ ��������� ��������.
	CorrectScale();

	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Pointf topLeftWorld = globalWorld.getAppearanceTopLeft();
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
				pPixelBufToDraw = std::make_shared<clan::PixelBuffer>(int(windowSize.width), int(windowSize.height), clan::tf_rgba8);
				pPixelBufToWrite = std::make_shared<clan::PixelBuffer>(int(windowSize.width), int(windowSize.height), clan::tf_rgba8);
				pImage = std::make_shared<clan::Image>(canvas, *pPixelBufToDraw, clan::Rectf(0, 0, windowSize));

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
		}

		// ������������ �������� �� �����.
		pImage->set_subimage(canvas, 0, 0, *pPixelBufToDraw, clan::Rect(0, 0, pPixelBufToDraw->get_size()));
		pImage->draw(canvas, 0, 0);
		// ��������� ������� ���������.
	}
	else {
		// ������ ��������.

		// ���������� �������������� ����� ����, �� ���� � ������ ������ � coordSystem.
		float xDotIndex = 0;
		float yDotIndex = 0;

		// ������� ������.
		clan::Rectf r;

		// ���������� �� �����, ��� �����������.
		bool showTextInCell = scale < cCompactCellDetailLevel;

		// ��������� ������� ���������.
		LocalCoord coordSystem(globalWorld.getCopyDotsArray(), topLeftWorld);

		// �������� �� ���������� ���������, ��� ��������.
		bool illuminated = getIlluminatedWorld();

		// ���� �����, ��� ����������� ���������� �������� ����.
		clan::Colorf color;

		// � ����� ���������, ���� �� ������ �� ������� ����.
		//
		while (r.top < windowSize.height) {

			r.top = r.bottom;
			r.bottom = (yDotIndex + 1) / scale;

			r.left = r.right = 0;
			xDotIndex = 0;

			while (r.left < windowSize.width) {

				// ����� ����. 
				const Dot &d = coordSystem.get_dot(xDotIndex, yDotIndex);

				// ������� ���� �����.
				d.get_color(color);

				// ���� �������� ���������, ������� �����-�����.
				if (illuminated)
					color.set_alpha(1);

				// ����� ��������� � ��������� ������, ��. �������� ++!.
				r.left = r.right;
				r.right = (xDotIndex++ + 1) / scale;

				// ������������ ������.
				canvas.fill_rect(r, color);

				// ������������ � ������������, ���� ����������.
				if (showTextInCell)
					DrawCellCompact(canvas, d, r, int(topLeftWorld.x + xDotIndex - 1), int(topLeftWorld.y + yDotIndex), color);
			}

			// ��������� � ���������� ���� ������.
			++yDotIndex;
		}

		// �������� �����.
		DrawGrid(canvas, windowSize);

		// ��������� ������ ���������.
	}

}

void ModelRender::DrawGrid(clan::Canvas &canvas, const clan::Sizef &windowSize)
{
	// ������������ �����.

	// �������.
	float scale = globalWorld.getAppearanceScale();

	// ���������� ����� ������� ��� �������� � �������� ��������. ���������� �����, ������������ � ����.
	clan::Sizef numLines(windowSize * scale);

	// ������������ �������������� � ������������ �����.
	for (float i = 1; i <= numLines.height; i++) {
		float y = i / scale;
		canvas.draw_line(0, y, windowSize.width, y, clan::Colorf::darkblue);
	}
	for (float i = 1; i <= numLines.width; i++) {
		float x = i / scale;
		canvas.draw_line(x, 0, x, windowSize.height, clan::Colorf::darkblue);
	}
}

void ModelRender::DrawCellCompact(clan::Canvas &canvas, const Dot &d, const clan::Rectf &rect, int xLabel, int yLabel, clan::Colorf color)
{
	// ������������ ������ � ���������� ���� - � ����������� � ���������, ������� ����������.

	// ��������� �����, ������� ������ �� � ��������� ������������, �������� ����� ��� ����������.
	float numLinesInRect = roundf(rect.get_height() / cCompactCellResLineHeight - 1);

	// ���������� ����� � ��������� - ���, ������� �������, ��� ������� ����������.
	float numLines = clan::min(float(globalWorld.getElemCount() + 2), numLinesInRect);

	// ������ ����� ��� ��������.
	const float indent = rect.left + 3;

	// ���� ������ ���� ����� ���� ������ � ����������� �� ����� ����� ������.
	color = (color.get_red() + color.get_green() + color.get_blue()) * color.get_alpha() < 1.5f ? clan::Colorf::white : clan::Colorf::black;

	// ������������ ��������� �������, ������������� � ���������� ���������.
	if (numLines > 0) {

		// ���������� ������.
		float yLine = rect.top + 16;

		cellFont.draw_text(canvas, indent, yLine, cSolar + '\t' + clan::StringHelp::float_to_text(d.getSolarEnergy() * 100) + "%", color);

		if (numLines > 1) {

			yLine += cCompactCellResLineHeight;
			cellFont.draw_text(canvas, indent, yLine, cEnergy + '\t' + clan::StringHelp::float_to_text(d.getGeothermalEnergy() * 100) + "%", color);
			yLine += cCompactCellResLineHeight;


			// � ����� ������������ ������ � ���������.
			for (int i = 0; i < numLines - 2; i++) {

				// �������� � ���������.
				//float percent = d.res[i];
				float percent = d.res[i] * 100 / globalWorld.getResMaxValue(i);

				cellFont.draw_text(canvas, indent, yLine, globalWorld.getResName(i) + '\t' + clan::StringHelp::float_to_text(percent) + "%", color);
				yLine += cCompactCellResLineHeight;
			}
		}
	}

	// ������������ � ������ ����� ���� ����������.
	cellFont.draw_text(canvas, indent, rect.bottom - 3, "X:Y " + clan::StringHelp::int_to_text(xLabel) + ":" + clan::StringHelp::int_to_text(yLabel), color);
}

// ������������ ������.
void ModelRender::draw(clan::Canvas &canvas)
{
	// For convenience and optimization get refs
	const clan::ViewGeometry &geom = geometry();

	// ������� �����.
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
	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Pointf topLeftWorld = globalWorld.getAppearanceTopLeft();
	float scale = globalWorld.getAppearanceScale();

	switch (e.button())
	{
	case clan::PointerButton::middle:

		// �������� ���� ��������� � ���������� �������
		isScrollStart = true;
		scrollWindow = e.pos(this);
		scrollWorld = topLeftWorld;
		break;

	case clan::PointerButton::wheel_up:

		// ���� ������ ������� Ctrl, ��������� �����, ���� Shift, ��������� ������, ����� �����������.
		if (e.shift_down())
			topLeftWorld.x -= max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			topLeftWorld.y -= max1(yWorldInc * scale);
		else
			Approach(e.pos(this), cScaleInc);

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
			ToDistance(e.pos(this), cScaleInc);
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
		clan::Pointf dif = scrollWindow - e.pos(this);

		// �� ������������ �������� ����� ����� ������� ���������� � ������ ��������.
		globalWorld.setAppearanceTopLeft(scrollWorld + dif * globalWorld.getAppearanceScale());

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
			const clan::Sizef worldSize = globalWorld.get_worldSize();

			// ��������� �����������-��������� �������.
			const float maxScale = std::min<float>(worldSize.width / windowSize.width, worldSize.height / windowSize.height);

			// ������������ �� ��������� ��� �� 1.
			scaleStep = float(pow(globalWorld.getAppearanceScale() / (maxScale < 1 ? maxScale : 1), 1.0 / 13));
		}
		else
			scaleStep = float(pow(globalWorld.getAppearanceScale() / cCompactCellDetailLevel, 1.0 / 12));


		// ��������� ��������.
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
	// ��������� ������� - ���������� �����������.
	//
	// �������, ��� ������� ��������� ����������, ������ �������� ��� ��������. ��� �����
	// ���������� �������������� ������������� ���������, �� ���� ���������� ���������� �����������
	// ��� �������� �� ��������� ��������, �������� �������, ���������� ����� ���������� �����������
	// � �� ��� ������ ������� ���������.
	//

	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Pointf topLeftWorld = globalWorld.getAppearanceTopLeft();
	float scale = globalWorld.getAppearanceScale();

	// ���������� ���� ��� �������� �� ��������������� (��� ����� ��������� topLeftWorld.x ��� �����������).
	float wx1 = pos.x * scale;
	float wy1 = pos.y * scale;

	// ��������� ������� ��� ����������� (����������) �����������.
	scale /= scaleStep;

	// ���������� ���� ��� �������� ����� ���������������.
	//
	float wx2 = pos.x * scale;
	float wy2 = pos.y * scale;

	topLeftWorld.x += int(wx1 - wx2);
	topLeftWorld.y += int(wy1 - wy2);

	// �������� ���������.
	globalWorld.setAppearanceTopLeft(topLeftWorld);
	globalWorld.setAppearanceScale(scale);
}


void ModelRender::ToDistance(const clan::Pointf &pos, float scaleStep)
{
	// ����������� ������� - �������� �����������.

	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Pointf topLeftWorld = globalWorld.getAppearanceTopLeft();
	float scale = globalWorld.getAppearanceScale();

	// ������ �������, ��������� �����������.
	scale *= scaleStep;

	//const clan::Rect &viewRect = geometry().content_box();
	const clan::Sizef viewSize = geometry().content_size() * scale;
	const clan::Sizef worldSize = globalWorld.get_worldSize();

	// ��������� �������� ������ ���� �� �������� ������������ ��������.
	if (viewSize.width <= worldSize.width && viewSize.height <= worldSize.height) {
		float wx1 = pos.x * scale / scaleStep;
		float wy1 = pos.y * scale / scaleStep;
		float wx2 = pos.x * scale;
		float wy2 = pos.y * scale;
		topLeftWorld.x += int(wx1 - wx2);
		topLeftWorld.y += int(wy1 - wy2);
	}

	// �������� ���������.
	globalWorld.setAppearanceTopLeft(topLeftWorld);
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
			LocalCoord coordSystem(globalWorld.getCopyDotsArray(), globalWorld.getAppearanceTopLeft());

			// �������� �� ���������� ���������, ��� ��������.
			bool illuminated = getIlluminatedWorld();

			// ������������ �������� �����.
			lock.unlock();

			// ��������� ������ �������������.

			// ��������� �� ����� ������.
			unsigned char *pixels = (unsigned char *)pPixelBufToWrite->get_data();

			// ���� �����, ��� ����������� ���������� �������� ����.
			clan::Colorf color;

			for (float ypos = 0; ypos < height; ypos++)
			{
				for (float xpos = 0; xpos < width; xpos++)
				{
					// ����� ����. ������ ����� ������ ������, ��� � ���������� ������� ����� ����� ���� ����������� �����, ���� ��� ����� ��� �����������.
					const Dot &d = coordSystem.get_dot(xpos * scale, ypos * scale);

					d.get_color(color);

					*(pixels++) = (unsigned char)(color.get_red() * 255);
					*(pixels++) = (unsigned char)(color.get_green() * 255);
					*(pixels++) = (unsigned char)(color.get_blue() * 255);
					*(pixels++) = illuminated ? 255 : (unsigned char)(color.get_alpha() * 255);
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
	const clan::Sizef earthSize = globalWorld.get_worldSize();

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
		clan::Pointf topLeftWorld = globalWorld.getAppearanceTopLeft();

		// ���� � ���������� ���������� ������� ���� �������� �� ������� ����, ���� ���������������� ������� ����������.
		// �� ��������� ������������, �� ����������� - ����������� ���������.
		if (topLeftWorld.x < 0.0f)
			topLeftWorld.x += earthSize.width;
		else if (topLeftWorld.x >= earthSize.width)
			topLeftWorld.x -= earthSize.width;

		if (topLeftWorld.y < 0.0f)
			topLeftWorld.y = 0.0f;
		else if (topLeftWorld.y + scaledSize.height > earthSize.height)
			topLeftWorld.y = earthSize.height - scaledSize.height;
		
		// ��������� ���������.
		globalWorld.setAppearanceTopLeft(topLeftWorld);
	}
}
