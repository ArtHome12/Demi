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

#include "organism.h"
#include "settings_storage.h"
#include "reactions.h"
#include "local_coord.h"


// ����������������� ���� � ����.
const int cDaysInYear = 365;

// ���������� ����� � ������, ���������� � �������� 60�*60�*24�=86400
const int cTicksInDay = 86400;
//const int cTicksInDay = 10;

// ���������� ���������� - ���.
class World;
extern World globalWorld;


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
	Dot *getDotsArray() { return arDots; }
	DemiTime getModelTime() { return timeBackup; }

	// ������ � ���������.
	clan::Sizef get_worldSize() { return worldSize; }
	int getElemCount() { return elemCount; }
	int getEnergyCount() { return energyCount; }
	float getLightRadius() { return lightRadius; }
	float getTropicHeight() { return tropicHeight; }
	float getResMaxValue(int index) { return arResMax[index]; }
	const std::string &getResName(int index) { return arResNames[index]; }
	const bool getResVisibility(int index) { return arResVisible[index]; }
	void setResVisibility(int index, bool visible) { arResVisible[index] = visible; }
	const clan::Pointf &getAppearanceTopLeft() { return appearanceTopLeft; }
	void setAppearanceTopLeft(const clan::Pointf newTopLeft) { appearanceTopLeft = newTopLeft; }
	float getAppearanceScale() { return appearanceScale; }
	void setAppearanceScale(float newScale) { appearanceScale = newScale; }
	std::shared_ptr<demi::Species> getSpecies() { return species; }

	// ������������� ��������.
	void setSettingsStorage(SettingsStorage* pSettingsStorage) { pSettings = pSettingsStorage; }

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
	
	// �������� ����������� �������� ��� ��� (��������� ������������ ������������� � ����������).
	bool *arResVisible = nullptr;

	// ������ ������������ �������� �������� ��� ����� ������������ �� ������.
	float *arResMax = nullptr;

	// ��������� ������� ��� ��������, �������� �� 0 �� 1.
	float* arResVolatility = nullptr;

	// ��������� ��������� - �� ���������� shared_ptr, ��� ��� � ������� ������ ����������� ~SettingsStorage ����������� 
	// ���������� ������ ��� ������������ - � ������ World ���������� � ����� ������� ����� ��.
	SettingsStorage* pSettings;

	// ��������� ���� ����������. ����������� ������, � ������� - ��� ��������������.
	std::shared_ptr<demi::Species> species;

	// ���������� �������, ��������� ��� ����������.
	std::map<std::string, std::shared_ptr<demi::ChemReaction>> reactions;

	// ���� ���������. ��� ������ - ����.
	std::shared_ptr<demi::Organism> animal;

	std::random_device random_device; // �������� ��������.
	std::mt19937 generator; // ��������� ��������� �����.
	
	// ��� ��������� ���������� �����. ����������� �������� ��� ��������
	std::uniform_int_distribution<> rnd_angle;

	// ��� ��������� ���������� �����. ���������� ���������� ��� ��������.
	std::uniform_int_distribution<> rnd_Coord;


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

	// ����������� ������� ��� ���������� ����� ����������.
	std::shared_ptr<demi::Species> DoReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor);

	// ����������� ������� ��� ������ ����� ����������.
	void DoWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies);

};




