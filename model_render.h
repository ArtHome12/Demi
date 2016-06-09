/* ===============================================================================
������������� �������� ������ ����.
��������� ��� ��������� ������.
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

	// ������������ ������.
	void draw(clan::Canvas &canvas);

	// �������� ��� ��������� ��������.
	void toggleIlluminated() { illuminated = !illuminated; }

	// ���������� ������ �������� ���� ����.
	const clan::Pointf &getTopLeftWorld() { return topLeftWorld; }
	void setTopLeftWorld(const clan::Pointf &newValue) { topLeftWorld = newValue; }

	// ������� ��������� ����.
	const float getScaleWorld() { return scale; }
	void setScaleWorld(float newValue) { scale = newValue; }

protected:
	/// Renders the content of a view
	virtual void render_content(clan::Canvas &canvas);

private:

	// ������ ������� ��� ��������� ������ (������������ ��� ���������, ���� ������ �� �������).
	clan::Sizef oldWindowSize;

	// ����� ��� ����������� ���� � ���� ����� ��� ��������������� ��������.
	std::shared_ptr<clan::PixelBuffer> pPixelBuf;

	// ����� ��� ��������� �� ������, ������������ ��� ��������� �������, ��� ��� �����������.
	std::shared_ptr<clan::Image> pImage;

	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Pointf topLeftWorld;
	float scale = 1;

	// ������, ���� �������� ���������.
	bool illuminated = false;

	// ������, ����� ������ ������� ������ ���� � �� � ������ ���������.
	bool isScrollStart = false;

	// ����� ������� ������ ���� � ������� ����������� � � �������.
	float scrollWindowX, scrollWindowY, scrollWorldX, scrollWorldY;

	void on_mouse_down(clan::PointerEvent &e);
	void on_mouse_up(const clan::PointerEvent &e);
	void on_mouse_move(const clan::PointerEvent &e);
	void on_mouse_dblclk(const clan::PointerEvent &e);

	// ��������� ������� - ���������� �����������.
	void Approach(const clan::Pointf &pos);

	// ����������� ������� - �������� �����������.
	void ToDistance(const clan::Pointf &pos);

	// ������������ �����.
	void DrawGrid(int width, int height);

	// ������������ ������ � ���������� ���� - � ����������� � ���������, ������� ����������.
	// � ������� ��������� ����� �����������, �������������, ��� � ���������� ���������� � ������� ���������� ����� ������������ ����.
	void DrawCellCompact(const Dot &d, const clan::Rectf &rect, int x, int y);

};

