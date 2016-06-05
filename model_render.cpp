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
#include "earth.h"
#include "model_render.h"


// �������, ��� ������� ���������� ������������ ����������� �� ������ �� ����������� ������.
const float cPixelDetailLevel = 0.15f;

// �������, ��� ������� ���������� ����������� ����� ������.
const float cLinesDetailLevel = 0.1f;

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

ModelRender::ModelRender()
{
	slots.connect(sig_pointer_press(), this, &ModelRender::on_mouse_down);
	slots.connect(sig_pointer_release(), this, &ModelRender::on_mouse_up);
	slots.connect(sig_pointer_move(), this, &ModelRender::on_mouse_move);
	slots.connect(sig_pointer_double_click(), this, &ModelRender::on_mouse_dblclk);
}


void ModelRender::render_content(clan::Canvas &canvas)
{
	// ������������ ������ � ��������� �����.
	//
	// ������� ������� ��� �����������.
	clan::Sizef	windowSize = geometry().content_size();

	// ���� ������ ��������, �������.
	if (windowSize.width == 0 || windowSize.height == 0)
		return;

	// ���� ������ ���������, ������������ ������������.
	if (oldWindowSize != windowSize) {
		oldWindowSize = windowSize;
		pPixelBuf = std::make_shared<clan::PixelBuffer> (int(windowSize.width), int(windowSize.height), clan::tf_rgba8);
		pImage = std::make_shared<clan::Image>(canvas, *pPixelBuf, clan::Rectf(0, 0, windowSize));
	}

	// ��� ��������, ������ � ������� �����������.
	const float scaledWidth = windowSize.width * scale;
	const float scaledHeight = windowSize.height * scale;

	// ��� �����������.
	const float earthWidth = globalEarth.get_worldWidth();
	const float earthHeight = globalEarth.get_worldHeight();

	// ���� ������ ���� ���� ������ ������������� ����, ���� ���������������� �������.
	if (scaledWidth > earthWidth || scaledHeight > earthHeight) {
		float w = earthWidth / windowSize.width;
		float h = earthHeight / windowSize.height;
		scale = w < h ? w : h;
	}
	else {

		// ���� � ���������� ���������� ������� ���� �������� �� ������� ����, ���� ���������������� ������� ����������.
		// �� ��������� ������������, �� ����������� - ����������� ���������.
		if (xWorld < 0)
			xWorld += earthWidth;
		else if (xWorld >= earthWidth)
			xWorld -= earthWidth;

		if (yWorld < 0)
			yWorld = 0;
		else if (yWorld + scaledHeight > earthHeight)
			yWorld = earthHeight - scaledHeight;
	}

	// ��������� ������� ���������.
	LocalCoord coordSystem(globalEarth.getCopyDotsArray(), xWorld, yWorld);

	// � ����������� �� ��������, ������������ ����� ���� ������.
	if (scale >= cPixelDetailLevel) {

		// ������ �������.

		// ���� �����, ��� ����������� ���������� �������� ����.
		clan::Colorf color;

		// ��������� �� ����� ������.
		pPixelBuf->lock(canvas, clan::access_write_only);
		unsigned char *pixels = (unsigned char *)pPixelBuf->get_data();

		for (int ypos = 0; ypos < windowSize.height; ypos++)
		{
			for (int xpos = 0; xpos < windowSize.width; xpos++)
			{
				// ����� ����. ������ ����� ������ ������, ��� � ���������� ������� ����� ����� ���� ����������� �����, ���� ��� ����� ��� �����������.
				Dot &d = coordSystem.get_dot(xpos * scale, ypos * scale);

				d.get_color(color);

				*(pixels++) = (unsigned char)(color.get_red() * 255);
				*(pixels++) = (unsigned char)(color.get_green() * 255);
				*(pixels++) = (unsigned char)(color.get_blue() * 255);
				*(pixels++) = illuminated ? 255 : (unsigned char)(color.get_alpha() * 255);
			}
		}

		// ������������ �������� �� �����. ������������ ��� ������ � ��������� ��� �������� � ����.
		pPixelBuf->unlock();
		pImage->set_subimage(canvas, 0, 0, *pPixelBuf, clan::Rect(0, 0, pPixelBuf->get_size()));
		pImage->draw(canvas, 0, 0);
		
		//
		// ��������� ����� ���������.
	}
	else {

/*		// ������ ��������.
		//

		// ���������� � ������� ������� ���������.
		//
		int xpos = 0;
		int ypos = 0;

		// ���������� ������.
		clan::Rectf r;

		// ���������� �� �����, ��� �����������.
		bool showTextInCell = scale < cCompactCellDetailLevel;

		// � ����� ���������, ���� �� ������ �� ������� ����.
		//
		while (r.top < windowHeight) {

			r.top = r.bottom;
			r.bottom = float((ypos + 1) / scale);

			r.left = r.right = 0;
			xpos = 0;

			while (r.left < windowWidth) {

				// ����� ����. 
				const demi::Dot &d = coord(xpos, ypos);

				// ������� ���� �����.
				d.get_color(color);

				// ���� �������� ���������, ������� �����-�����.
				if (illuminated)
					color.set_alpha(1);

				// ����� ��������� � ��������� ������, ��. �������� ++!.
				r.left = r.right;
				r.right = float((xpos++ + 1) / scale);

				// ������������ ������.
				pCanvas->fill_rect(r, color);

				// ������������ � ����������, ���� ����������.
				if (showTextInCell)
					DrawCellCompact(d, r, xpos - 1, ypos);
			}

			// ��������� � ���������� ���� ������.
			++ypos;
		}

		// �������� �����, ���� ��� ����������.
		//
		if (scale < cLinesDetailLevel)
			DrawGrid(windowWidth, windowHeight);

		//
		// ��������� ������ ���������.
		*/
	}

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
	//clan::InputDevice &keyboard = pMainWindow->get_ic().get_keyboard();

	switch (e.button())
	{
	case clan::PointerButton::middle:

		// �������� ���� ��������� � ���������� �������
		//
		isScrollStart = true;
		scrollWindowX = e.pos(this).x;
		scrollWindowY = e.pos(this).y;
		scrollWorldX = xWorld;
		scrollWorldY = yWorld;

		// ����������� ������� ����, ����� ������ �� ���������� ������.
		//pMainWindow->capture_mouse(true);

		break;
	case clan::PointerButton::wheel_up:

		// ���� ������ ������� Ctrl, ��������� �����, ���� Shift, ��������� ������, ����� �����������.
		//
		if (e.shift_down())
			xWorld -= max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			yWorld -= max1(yWorldInc * scale);
		else
			Approach(e.pos(this));

		// �������� ��������� � ��������.
		//updateScaleSize();
		break;

	case clan::PointerButton::wheel_down:
		if (e.shift_down())
			xWorld += max1(xWorldInc * scale);
		else if (e.ctrl_down() || e.cmd_down())
			yWorld += max1(yWorldInc * scale);
		else
			ToDistance(e.pos(this));

		//updateScaleSize();
		break;
	default:
		break;
	}
}

void ModelRender::on_mouse_up(const clan::PointerEvent &e)
{

}

void ModelRender::on_mouse_move(const clan::PointerEvent &e)
{

}

void ModelRender::on_mouse_dblclk(const clan::PointerEvent &e)
{

}

void ModelRender::Approach(const clan::Pointf &pos)
{
	// ��������� ������� - ���������� �����������.
	//
	// �������, ��� ������� ��������� ����������, ������ �������� ��� ��������. ��� �����
	// ���������� �������������� ������������� ���������, �� ���� ���������� ���������� �����������
	// ��� �������� �� ��������� ��������, �������� �������, ���������� ����� ���������� �����������
	// � �� ��� ������ ������� ���������.
	//

	// ���������� ���� ��� �������� �� ��������������� (��� ����� ��������� xWorld ��� �����������).
	//
	float wx1 = pos.x * scale;
	float wy1 = pos.y * scale;

	// ��������� ������� ��� ����������� (����������) �����������.
	scale /= cScaleInc;

	// ���������� ���� ��� �������� ����� ���������������.
	//
	float wx2 = pos.x * scale;
	float wy2 = pos.y * scale;

	xWorld += int(wx1 - wx2);
	yWorld += int(wy1 - wy2);
}


void ModelRender::ToDistance(const clan::Pointf &pos)
{
	// ����������� ������� - �������� �����������.
	//
	// ������ �������, ��������� �����������.
	scale *= cScaleInc;

	const clan::Rect &viewRect = geometry().content_box();

	// ��������� �������� ������ ���� �� �������� ������������ ��������.
	if (viewRect.get_width() * scale <= globalEarth.get_worldWidth() && viewRect.get_height() * scale <= globalEarth.get_worldHeight()) {
		float wx1 = pos.x * scale / cScaleInc;
		float wy1 = pos.y * scale / cScaleInc;
		float wx2 = pos.x * scale;
		float wy2 = pos.y * scale;
		xWorld += int(wx1 - wx2);
		yWorld += int(wy1 - wy2);
	}
}

