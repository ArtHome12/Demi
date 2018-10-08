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

#include <set>
#include "organism.h"
#include "settings_storage.h"
#include "reactions.h"
#include "local_coord.h"
#include "amounts.h"
#include "demi_time.h"


// ���������� ���������� - ���.
class World;
extern World globalWorld;


//
// �������� ��������� �������
//
class Solar {
private:

	// ���������� ������� ��� ������ � ����������� �� �������.
	clan::Point getPos(const demi::DemiTime &timeModel);

public:

	// �������� �������� ��� ���������� ������� ������� �����.
	void Shine(const demi::DemiTime &timeModel);
};


//
// �������� ������������� �������
//
class Geothermal {
public:
	clan::Point coord;
};


//
// ������ �����������, ��������� ������ �����.
//
class World {

//	friend Dot;
//	friend Solar;

public:
	World();
	~World();

	// ��������� ������ �� xml-����� � ���������.
	void loadModel(const std::string &filename);
	void saveModel(const std::string &filename);

	// ���������������� ��� ���������� ������ ������
	void runEvolution(bool is_active);

	// �������� ������ ������.
	void resetModel(const std::string &modelFilename, const std::string &defaultFilename);

	// ���������� ����� �����������.
	Dot *getDotsArray() { return arDots; }
	const demi::DemiTime& getModelTime() { return timeBackup; }

	// ������ � ���������.
	clan::Size get_worldSize() { return worldSize; }
	size_t getElemCount() { return elemCount; }
	size_t getEnergyCount() { return energyCount; }
	size_t getLightRadius() { return lightRadius; }
	size_t getTropicHeight() { return tropicHeight; }
	unsigned long long getResMaxValue(size_t index) { return arResMax[index]; }
	const clan::Color &getResColors(size_t index) { return arResColors[index]; }
	const std::string &getResName(size_t index) { return arResNames[index]; }
	const bool getResVisibility(size_t index) { return arResVisible[index]; }
	void setResVisibility(size_t index, bool visible) { arResVisible[index] = visible; }
	const clan::Point &getAppearanceTopLeft() { return appearanceTopLeft; }
	void setAppearanceTopLeft(const clan::Point& newTopLeft) { appearanceTopLeft = newTopLeft; }
	float getAppearanceScale() { return appearanceScale; }
	void setAppearanceScale(float newScale) { appearanceScale = newScale; }
	std::shared_ptr<demi::Species> getSpecies() { return species; }
	const std::shared_ptr<demi::ChemReaction> getReaction(size_t index) { return reactions.at(index); }

	std::mt19937 &getRandomGenerator() { return generator; }

	// ������������� ��������.
	void setSettingsStorage(SettingsStorage* pSettingsStorage) { pSettings = pSettingsStorage; }
	SettingsStorage*getSettingsStorage() { return pSettings; }

	// ���������� ���������� ����� �� ���������� �������.
	clan::Point getDotXYFromIndex(size_t index);

	// ���������� ������ ����� ��� ��������� ���������.
	size_t getDotIndexFromXY(size_t x, size_t y) { return x + y * worldSize.width; }

	// ������ ��� �������� ���������� ���������. ��������� ��������� ������� ����������, ������� ������� ��� � ������ ��� ��������.
	Amounts amounts;

private:

	// ������� ������ ������� � ��������� ������ � � ���������� ��� ��������� �� ������ �����.
	demi::DemiTime timeModel;
	demi::DemiTime timeBackup;

	// �������� ��������� �������.
	Solar solar;

	// ���������� ������������� ����������.
	size_t energyCount = 0;

	// ������������� ��������� - ������.
	Geothermal *arEnergy = nullptr;

	// ������� ����.
	clan::Size worldSize;

	// ����������� ����� � ����� ��� ����������� �� ������.
	Dot *arDots;

	// ��������, ������������ ��� ����������� ������ - ���������� ������ �������� ����, �������.
	clan::Point appearanceTopLeft;
	float appearanceScale = 1.0f;

	// ������������ ������ ������������ ������ ��������������� ������, ���������� 90% ������ ����.
	// ��� �� �������� � ������������ ������� ������ - � ����� �������� ����� ��� ���� ����������� � ������ 1, �������������� � ������ - ����� �������.
	size_t lightRadius = 0;

	// ��� ������ �� ���������, ������������� �������� ������ ���, �������� sin(23.5)=0.4 ��� �� 10% � ������ ������� �� ��������.
	size_t tropicHeight = 0;

	// ����� ���������.
	clan::Color* arResColors = nullptr;

	// ���������� ���������� ���������, ������������ � ������.
	size_t elemCount = 0;

	// �������� ��������. ���������������� �����.
	std::string *arResNames = nullptr;
	
	// �������� ����������� �������� ��� ��� (��������� ������������ ������������� � ����������).
	bool *arResVisible = nullptr;

	// ������ ������������ �������� �������� ��� ����� ������������ �� ������.
	unsigned long long *arResMax = nullptr;

	// ��������� ������� ��� ��������, �������� �� 0 �� 1.
	float* arResVolatility = nullptr;

	// ��������� ��������� - �� ���������� shared_ptr, ��� ��� � ������� ������ ����������� ~SettingsStorage ����������� 
	// ���������� ������ ��� ������������ - � ������ World ���������� � ����� ������� ����� ��.
	SettingsStorage* pSettings;

	// ��������� ���� ����������. ����������� ������, � ������� - ��� ��������������.
	std::shared_ptr<demi::Species> species;

	// ���������� �������, ��������� ��� ����������.
	std::vector<std::shared_ptr<demi::ChemReaction>> reactions;

	// ���� ���������.
	std::vector<demi::Organism*> animals;

	std::random_device random_device; // �������� ��������.
	std::mt19937 generator; // ��������� ��������� �����.
	
	// ��� ��������� ���������� �����. ����������� �������� ��� ��������
	std::uniform_int_distribution<> rnd_angle;

	// ��� ��������� ���������� �����. ���������� ���������� ��� ��������.
	std::uniform_int_distribution<> rnd_Coord;

	// ������� �������������� ��� �������������� ��� ������ ���.
	clan::Point LUCAPos;

	// �������� ���������.
	void makeTick();

	// �������� ��������.
	void diffusion();

	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	void fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect);

	// ����� �������������� ���������� ������������� �������.
	void addGeothermal(size_t i, const clan::Point &coord);

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

	// ����������� ������� ��� ���������� ����� ����������. �������������� �� ������ �������.
	//std::shared_ptr<demi::Species> doReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor);
	// ����������� ������� ��� ������ ����� ����������, ����������� ������ ������� �������� �����.
	//void doWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies, std::set<std::string> &dict);

	// �������� �� SaveModel() ��� ��������. ������ ������ ���������.
	void doWriteOrganism(clan::File &binFile, std::set<std::string> &dict, demi::Organism* organism);
	demi::Organism* doReadOrganism(clan::File &binFile, std::set<std::string> &dict, const clan::Point &center);

	// �������������� ������ ���������� �� ������ ��������� ��������� � ������, ������������ ����� ��������.
	void InitResMaxArray();
};




