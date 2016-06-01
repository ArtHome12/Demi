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
//const int cTicksInDay = 1000;

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
	// ����� ��������� ���������, ���������� � ���������� �����������.
	int xcenter, ycenter;

	LocalCoord(int x, int y) : xcenter(x), ycenter(y) {};

	Dot& operator()( int x, int y ) const;
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

	// ���������� ������� ��� ������ � ����������� �� ��� ���� (�� ��������� ����).
	unsigned int YPos(const DemiTime &timeModel);
	// ���������� ������� ��� ������ � ����������� �� ������� ����� (�� ����������� ����).
	unsigned int XPos(const DemiTime &timeModel);

public:

	// �������� �������� ��� ���������� ������� ������� �����.
	void Shine(const DemiTime &timeModel);
};


//
// �������� ������������� �������
//
class Geothermal {
public:
	unsigned int XPos;
	unsigned int YPos;
};


//
// ������ �����������, ��������� ������ �����.
//
class Earth {

	friend Dot;
	friend LocalCoord;

public:
	Earth();
	~Earth();

	// ��������� ������ �� xml-����� � ���������.
	void LoadModel(const std::string &filename);
	void SaveModel(const std::string &filename);

	// ���������������� ��� ���������� ������ ������
	void RunEvolution(bool is_active);

	// ���������� ����� �����������.
	Dot *getCopyDotsArray() { return arDotsCopy; }
	DemiTime getModelTime() { return timeBackup; }

	// ������ � ���������.
	int get_worldWidth() { return worldWidth; }
	int get_worldHeight() { return worldHeight; }
	int get_elemCount() { return elemCount; }
	int get_energyCount() { return energyCount; }
	int get_lightRadius() { return lightRadius; }
	int get_tropicHeight() { return tropicHeight; }

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
	int worldWidth = 0;
	int worldHeight = 0;

	// ���������� ���������� ���������, ������������ � ������.
	int elemCount = 0;

	// ���������� ������������� ����������.
	int energyCount;

	// ������������ ������ ������������ ������ ��������������� ������, ���������� 90% ������ ����.
	// ��� �� �������� � ������������ ������� ������ - � ����� �������� ����� ��� ���� ����������� � ������ 1, �������������� � ������ - ����� �������.
	int lightRadius = 0;

	// ��� ������ �� ���������, ������������� �������� ������ ���, �������� sin(23.5)=0.4 ��� �� 10% � ������ ������� �� ��������.
	int tropicHeight = 0;

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
	void FillRectResource(int resId, float amount, const clan::Rect &rect);

	// ����� �������������� ���������� ������������� �������.
	void AddGeothermal(int i, int x, int y);

private:

	bool thread_exit_flag = false;		// ���� ������, ����� ������ �����������.
	bool thread_run_flag = false;		// ���� ������, �� ����� �������� � �������� ������, ����� �����������.
	bool thread_crashed_flag = false;	// ���� ������, ������ ����� ���������� ��������.
	//bool thread_complete_flag = false;	// ���� ������, ������ ����� �������� ������ ������ (��������������� � ��������� ������ ��� ��������� ������).
	std::mutex thread_mutex;
	std::condition_variable threadMainToWorkerEvent;
	//std::condition_variable threadWorkerToMainEvent;

	// ������� ������� ������, ������������ ������.
	void worker_thread();
};




