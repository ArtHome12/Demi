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
	ModelRender(std::shared_ptr<SettingsStorage> &pSettingsStorage);

	// ������������ ������.
	void draw(clan::Canvas &canvas);

	// ���������� ������ �������� ���� ����.
	const clan::Pointf &getTopLeftWorld() { return topLeftWorld; }
	void setTopLeftWorld(const clan::Pointf &newValue) { topLeftWorld = newValue; }

	// ������� ��������� ����.
	const float getScaleWorld() { return scale; }
	void setScaleWorld(float newValue) { scale = newValue; }

	// ���������� ��������� ����.
	const bool getIlluminatedWorld() { return pSettings->getTopMenuIsModelIlluminated(); }
	void setIlluminatedWorld(bool newValue);

protected:
	/// Renders the content of a view
	virtual void render_content(clan::Canvas &canvas);

private:
	// ���������.
	std::shared_ptr<SettingsStorage> &pSettings;

	// ������ ������� ��� ��������� ������ (������������ ��� ���������, ���� ������ �� �������).
	clan::Sizef oldWindowSize;

	// ����� ��� ����������� ���� � ���� ����� ��� ��������������� ��������.
	std::shared_ptr<clan::PixelBuffer> pPixelBuf;

	// ����� ��� ��������� �� ������, ������������ ��� ��������� �������, ��� ��� �����������.
	std::shared_ptr<clan::Image> pImage;

	// ����� ��� ��������� �������� � ������.
	clan::Font cellFont;

	// ������� ���������� ������ �������� ���� ���� � �������.
	clan::Pointf topLeftWorld;
	float scale = 1;

	// ������, ����� ������ ������� ������ ���� � �� � ������ ���������.
	bool isScrollStart = false;

	// ����� ������� ������ ���� � ������� ����������� � � �������.
	clan::Pointf scrollWindow;
	clan::Pointf scrollWorld;

	void on_mouse_down(clan::PointerEvent &e);
	void on_mouse_up(const clan::PointerEvent &e);
	void on_mouse_move(const clan::PointerEvent &e);
	void on_mouse_dblclk(const clan::PointerEvent &e);

	// ��������� ������� - ���������� �����������.
	void Approach(const clan::Pointf &pos, float scaleStep);

	// ����������� ������� - �������� �����������.
	void ToDistance(const clan::Pointf &pos, float scaleStep);

	// ������������ �����.
	void DrawGrid(clan::Canvas &canvas, const clan::Sizef &windowSize);

	// ������������ ������ � ���������� ���� - � ����������� � ���������, ������� ����������.
	// � ������� ��������� ����� �����������, �������������, ��� � ���������� ���������� � ������� ���������� ����� ������������ ����.
	void DrawCellCompact(clan::Canvas &canvas, const Dot &d, const clan::Rectf &rect, int xLabel, int yLabel);

	// ����� ��������� � ���������� ���������.
	clan::SoundBuffer soundIlluminateOn;
	clan::SoundBuffer soundIlluminateOff;

};

