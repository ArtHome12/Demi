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

// ���������� ���������� - ��������� �� ���.
class Earth;
extern Earth globalEarth;


//
// ����� �� ������ ����������� �� ������� �������� � ��������� �� ����������.
//
class Dot {
public:
	
	// ��������� ������� � �����.
	float solarEnergy = 0;

	// ������������� ������� � �����.
	float energy = 0;

	// ��������� � ��� �������
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
	LocalCoord(Dot *arDots, const clan::Pointf &coord) : dots(arDots), center(coord) {};

	Dot& get_dot(float x, float y) const;

private:
	// ������ � ������� �����������.
	Dot *dots;

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
class Earth {

	friend Dot;
	friend Solar;

public:
	Earth();
	~Earth();

	// ��������� ������ �� xml-����� � ���������.
	void LoadModel(const std::string &filename);
	void SaveModel(const std::string &filename);

	// ���������������� ��� ���������� ������ ������
	void RunEvolution(bool is_active);

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

private:

	// ������� ������ ������� � ��������� ������ � � ���������� ��� ��������� �� ������ �����.
	DemiTime timeModel;
	DemiTime timeBackup;

	// �������� ��������� �������.
	Solar solar;

	// ������������� ��������� - ������.
	Geothermal *arEnergy = nullptr;

	// �������� ���������.
	void MakeTick();

	// �������� ��������.
	void Diffusion();


	// ������� ����.
	clan::Sizef worldSize;

	// ���������� ���������� ���������, ������������ � ������.
	int elemCount = 0;

	// ���������� ������������� ����������.
	int energyCount = 0;

	// ������������ ������ ������������ ������ ��������������� ������, ���������� 90% ������ ����.
	// ��� �� �������� � ������������ ������� ������ - � ����� �������� ����� ��� ���� ����������� � ������ 1, �������������� � ������ - ����� �������.
	float lightRadius = 0;

	// ��� ������ �� ���������, ������������� �������� ������ ���, �������� sin(23.5)=0.4 ��� �� 10% � ������ ������� �� ��������.
	float tropicHeight = 0;

	// ����������� ����� � ����� ��� ����������� �� ������.
	Dot *arDots;
	Dot *arDotsCopy;

	// ����� ���������.
	clan::Colorf* arResColors = nullptr;

	// ������ ������������ �������� �������� ��� ����� ������������ �� ������.
	float *arResMax = nullptr;

	// �������� ��������. ���������������� �����.
	std::string *arResNames = nullptr;
	
	// ��������� ������� ��� ��������, �������� �� 0 �� 1.
	float* arResVolatility = nullptr;

	// ������� �����
	std::thread thread;

	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	void FillRectResource(int resId, float amount, const clan::Rectf &rect);

	// ����� �������������� ���������� ������������� �������.
	void AddGeothermal(int i, const clan::Pointf &coord);

private:

	bool thread_exit_flag = false;		// ���� ������, ����� ������ �����������.
	bool thread_run_flag = false;		// ���� ������, �� ����� �������� � �������� ������, ����� �����������.
	bool thread_crashed_flag = false;	// ���� ������, ������ ����� ���������� ��������.
	std::mutex thread_mutex;
	std::condition_variable threadMainToWorkerEvent;

	// ������� ������� ������, ������������ ������.
	void worker_thread();
};




