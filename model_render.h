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
	~ModelRender();

	// ������������ ������.
	void draw(clan::Canvas &canvas);

	// ���������� ��������� ����.
	const bool getIlluminatedWorld() { return pSettings->getTopMenuIsModelIlluminated(); }
	void setIlluminatedWorld(bool newValue);

protected:
	/// Renders the content of a view
	void render_content(clan::Canvas &canvas) override;

private:
	// ���������.
	std::shared_ptr<SettingsStorage> &pSettings;

	// ������ ������� ��� ��������� ������ (������������ ��� ���������, ���� ������ �� �������).
	clan::Sizef oldWindowSize;

	// ����� ��� ����������� ���� � ���� ����� ��� ��������������� ��������.
	std::shared_ptr<clan::PixelBuffer> pPixelBufToWrite;
	std::shared_ptr<clan::PixelBuffer> pPixelBufToDraw;

	// ����� ��� ��������� �� ������, ������������ ��� ��������� �������, ��� ��� �����������.
	std::shared_ptr<clan::Image> pImage;

	// ����� ��� ��������� �������� � ������.
	clan::Font cellFont;

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

	// �����, ��������� ������������.
	std::thread thread;

	// ����� ������, ���������� ������������.
	bool threadExitFlag = false;		// ���� ������, ����� ������ �����������.
	bool threadRunFlag = false;		// ���� ������, �� ����� �������� � �������� ������, ����� �����������.
	bool threadCrashedFlag = false;	// ���� ������, ������ ����� ���������� ��������.
	std::mutex threadMutex;
	std::condition_variable threadEvent;

	// ������� ������� ������, ������������ ������.
	void workerThread();

	// ������������ ������� � ������� ����� ���� ������ ����� ��������� ��������.
	void CorrectScale();
};

