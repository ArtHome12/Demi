/* ===============================================================================
	������������� �������� ������ ����.
	������ ��� �������� ����������� - �����, ������������.
	24 august 2013.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once


// ����������������� ���� � ����.
const int cDaysInYear = 365;

// ���������� ����� � ������, ���������� � �������� 60�*60�*24�=86400
const int cTicksInDay = 86400;
//const int cTicksInDay = 10;

// ���������� ���������� - ���.
class World;
extern World globalWorld;


//
// ����� �� ������ ����������� �� ������� �������� � ��������� �� ����������.
//
class Dot {
public:
	
	// ��������� ������� � �����.
	float getSolarEnergy() const { return res[0]; }
	void setSolarEnergy(float newVal) { res[0] = newVal; }

	// ������������� ������� � �����.
	float getGeothermalEnergy() const { return res[1]; }
	void setGeothermalEnergy(float newVal) { res[1] = newVal; }

	// ����� ������ � ������ ��� ������ - ������ ���������� �������� ���� ��������� � ������������� �������.
	static int getSizeInMemory();

	// ��������� � ��� ������� - ������ ��� �������� ��� ��������� � ������������� ������� (��� ��������� ����������).
	float *res;

	// �����������, ������ ����� ��������.
	Dot();
	~Dot();

	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	void get_color(clan::Colorf &aValue) const;
};


//
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
//
class LocalCoord { 
public:
	LocalCoord(Dot *arDots, const clan::Pointf &coord);

	Dot& get_dot(float x, float y) const;

private:
	// ������ � ������� �����������.
	Dot *dots;

	// ������� ���� �� ������ ��������, ��� ����������.
	float worldWidth;
	float worldHeight;

	// ����� ��������� ���������, ���������� � ���������� �����������.
	clan::Pointf center;
};


//
// ������ �������� �������
//
class DemiTime {
public:
	// ���, �� 1 �� �������������.
	unsigned int year = 1;

	// ���� ����, �� 1 �� cDaysInYear ������������.
	unsigned int day = 1;

	// ������ ������ ���, �� 0 �� cTicksInDay
	unsigned int sec = 0;

	// ���������� ���� ���.
	void MakeTick();

	// ���������� ������ � ��������.
	std::string getDateStr() const;

	const bool operator!=(const DemiTime& rv) const {
		return (year != rv.year) || (day != rv.day) || (sec != rv.sec);
	}

	DemiTime &operator=(const DemiTime& rv) {
		year = rv.year;
		day = rv.day;
		sec = rv.sec;
		return *this;
	}
};

//
// �������� ��������� �������
//
class Solar {
private:

	// ���������� ������� ��� ������ � ����������� �� �������.
	clan::Pointf getPos(const DemiTime &timeModel);

public:

	// �������� �������� ��� ���������� ������� ������� �����.
	void Shine(const DemiTime &timeModel);
};


//
// �������� ������������� �������
//
class Geothermal {
public:
	clan::Pointf coord;
};


//
// ������ �����������, ��������� ������ �����.
//
class World {

	friend Dot;
	friend Solar;

public:
	World();
	~World();

	// ��������� ������ �� xml-����� � ���������.
	void LoadModel(const std::string &filename);
	void SaveModel(const std::string &filename);

	// ���������������� ��� ���������� ������ ������
	void RunEvolution(bool is_active);

	// �������� ������ ������.
	void ResetModel(const std::string &modelFilename, const std::string &defaultFilename);

	// ���������� ����� �����������.
	//Dot *getCopyDotsArray() { return arDotsCopy; }
	Dot *getCopyDotsArray() { return arDots; }
	DemiTime getModelTime() { return timeBackup; }

	// ������ � ���������.
	clan::Sizef get_worldSize() { return worldSize; }
	int getElemCount() { return elemCount; }
	int getEnergyCount() { return energyCount; }
	float getLightRadius() { return lightRadius; }
	float getTropicHeight() { return tropicHeight; }
	float getResMaxValue(int index) { return arResMax[index]; }
	const std::string &getResName(int index) { return arResNames[index]; }
	const clan::Pointf &getAppearanceTopLeft() { return appearanceTopLeft; }
	void setAppearanceTopLeft(const clan::Pointf newTopLeft) { appearanceTopLeft = newTopLeft; }
	float getAppearanceScale() { return appearanceScale; }
	void setAppearanceScale(float newScale) { appearanceScale = newScale; }

private:

	// ������� ������ ������� � ��������� ������ � � ����������� ��� ��������� �� ������ �����.
	DemiTime timeModel;
	DemiTime timeBackup;

	// �������� ��������� �������.
	Solar solar;

	// ���������� ������������� ����������.
	int energyCount = 0;

	// ������������� ��������� - ������.
	Geothermal *arEnergy = nullptr;

	// ������� ����.
	clan::Sizef worldSize;

	// ����������� ����� � ����� ��� ����������� �� ������.
	Dot *arDots;
	Dot *arDotsCopy;

	// ��������, ������������ ��� ����������� ������ - ���������� ������ �������� ����, �������.
	clan::Pointf appearanceTopLeft;
	float appearanceScale = 1.0f;

	// ������������ ������ ������������ ������ ��������������� ������, ���������� 90% ������ ����.
	// ��� �� �������� � ������������ ������� ������ - � ����� �������� ����� ��� ���� ����������� � ������ 1, �������������� � ������ - ����� �������.
	float lightRadius = 0;

	// ��� ������ �� ���������, ������������� �������� ������ ���, �������� sin(23.5)=0.4 ��� �� 10% � ������ ������� �� ��������.
	float tropicHeight = 0;

	// ����� ���������.
	clan::Colorf* arResColors = nullptr;

	// ���������� ���������� ���������, ������������ � ������.
	int elemCount = 0;

	// �������� ��������. ���������������� �����.
	std::string *arResNames = nullptr;
	
	// ������ ������������ �������� �������� ��� ����� ������������ �� ������.
	float *arResMax = nullptr;

	// ��������� ������� ��� ��������, �������� �� 0 �� 1.
	float* arResVolatility = nullptr;

	// �������� ���������.
	void MakeTick();

	// �������� ��������.
	void Diffusion();

	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	void FillRectResource(int resId, float amount, const clan::Rectf &rect);

	// ����� �������������� ���������� ������������� �������.
	void AddGeothermal(int i, const clan::Pointf &coord);

	// ������� �����
	std::thread thread;

	// ����� ��� ���������� �������.
	bool threadExitFlag = false;		// ���� ������, ����� ������ �����������.
	bool threadRunFlag = false;		// ���� ������, �� ����� �������� � �������� ������, ����� �����������.
	bool threadCrashedFlag = false;	// ���� ������, ������ ����� ���������� ��������.
	std::mutex threadMutex;
	std::condition_variable threadEvent;

	// ������� ������� ������, ������������ ������.
	void workerThread();
};



