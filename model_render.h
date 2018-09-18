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

	// ���������� ��������� ����.
	const bool getIlluminatedWorld() { return pSettings->getTopMenuIsModelIlluminated(); }
	void setIlluminatedWorld(bool newValue);

	const bool getIsFrameUpdated() { return isFrameUpdated; }

protected:
	/// Renders the content of a view
	void render_content(clan::Canvas &canvas) override;

private:
	// ���������.
	std::shared_ptr<SettingsStorage> &pSettings;

	// ������ ������� ��� ��������� ������ (������������ ��� ���������, ���� ������ �� �������).
	clan::Size oldWindowSize;

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
	clan::Point scrollWindow;
	clan::Point scrollWorld;

	// ������� ��� ��������� � ������������� �������.
	std::string solarTitle;
	std::string geothermalTitle;

	// ����, ��������������� ��� ����������� ����������� fps � App::update() � ������, ���� �������� �� ����������, � ����� ������ �����.
	// ����������� � render_content().
	bool isFrameUpdated = false;

	void on_mouse_down(clan::PointerEvent &e);
	void on_mouse_up(const clan::PointerEvent &e);
	void on_mouse_move(const clan::PointerEvent &e);
	void on_mouse_dblclk(const clan::PointerEvent &e);

	// ����������� ������� - �������� �����������.
	void DoScaleStep(const clan::Pointf &pos, float scaleStep, clan::Point &newTopLeft);

	// ������������ �����.
	void DrawGrid(clan::Canvas &canvas, const clan::Size &windowSize);

	// ������������ ������ � ���������� ���� - � ����������� � ���������, ������� ����������.
	void DrawCellCompact(clan::Canvas &canvas, const Dot &d, const clan::Rect &rect, int xLabel, int yLabel, const clan::Color& dotColor);

	// ����� ��������� � ���������� ���������.
	clan::SoundBuffer soundIlluminateOn;
	clan::SoundBuffer soundIlluminateOff;

	// �����, ��������� ������������.
	std::thread thread;

	// ������ 1 ������� ������� ������.
	int cellFontSymbolWidth = 0;

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

