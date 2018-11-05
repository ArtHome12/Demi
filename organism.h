/* ===============================================================================
	������������� �������� ������ ����.
	������ ��� ������ ���������.
	10 april 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once

#include "local_coord.h"
#include "demi_time.h"
#include "gene.h"

namespace demi {

	// ���� ������ - ����, ��������, �����, ���, �����, ���, �����.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

	// ��������� ��������������� ����������.
	class Organism;
	class DemiTime;
	class ChemReaction;


//
// ������, ������� ���.
//
class GenericCell
{
public:
	// �������������� ������ � ������� ��������� ���������.
	int x, y;

	GenericCell() : x(0), y(0), organism(nullptr) {}
	GenericCell(int Ax, int Ay, Organism * Aorganism) : x(Ax), y(Ay), organism(Aorganism) {}


	//virtual ~GenericCell(void);

	// ���������� ��� ������.
	virtual CellTypes getCellType() abstract;

	// ���������� ����� ������.
	virtual GenericCell *getClone() abstract;

	// ���� ������� ������ �� ��������.
	Organism *organism = nullptr;
};


//
// ������ - �����.
class CellAbdomen : public GenericCell
{
	// ���������� ��� ������.
	virtual CellTypes getCellType() override { return cellAbdomen; }

	// ���������� ����� ������.
	virtual GenericCell *getClone() override { return new CellAbdomen(*this); }

	// ������ ������� 
};


//
// ��������.
//
class Organism
{
public:
	Organism(const clan::Point &Acenter, uint8_t Aangle, int32_t Avitality, const DemiTime& Abirthday, uint64_t AancestorsCount, const std::shared_ptr<Species>& species);
	~Organism();

	// ������ � �����.
	uint8_t getAngle() const { return angle; }
	const LocalCoord& getCenter() const { return center; }
	const std::vector<std::shared_ptr<GenericCell>>& getCells() const { return cells; }
	const DemiTime& getBirthday() const { return birthday; }
	uint64_t getAncestorsCount() const { return ancestorsCount; }
	const std::shared_ptr<Species>& getSpecies() const { return ourSpecies; }
	organismAmounts_t& getLeftReagentAmounts()  { return leftReagentAmounts; }
	int32_t getVitality() const { return vitality; }
	const std::shared_ptr<ChemReaction>& getChemReaction() const { return ourReaction; }

	// ����������� ������� ����������� ��� �������� ������ � ��� ���������.
	static uint8_t minActiveMetabolicRate, minInactiveMetabolicRate;

	// ��������� �������, ���� ������� �������� �����������.
	static int32_t desintegrationVitalityBarrier;

	// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
	// ���������� ������, ���� ����� ��������.
	bool makeTick();

	// ������������� ��������� ������� (������������ ����� �������).
	int32_t halveVitality() { return vitality /= 2; }

	// ������������ ������� �� �������� �� ������� ������� �����, ���������� ������, ���� ����� �������������.
	bool canMove() { return true; }

	// ������������ ������� �� ����� �� ������� ���������.

	// ����������� ����������� ��������� � ������ �����.
	void moveTo(const clan::Point &newCenter);

	// ������, ���� �������� ���. ���� ���, �� ��� ���� ��������� �� ������ �����.
	bool isAlive() { return vitality > 0; }

	// ������, ���� �������� ��� ���������� � ��� ���� ����������.
	bool needDesintegration() { return vitality < desintegrationVitalityBarrier; }

	// �������� ��������� ������� �� ���������� ��������� � ��������� ����������.
	void processInactiveVitality();

	// ��������� � ��������� ���� � ����.
	static Organism* createFromFile(clan::IODevice& binFile, const clan::Point& Acenter, const std::shared_ptr<Species>& Aspecies);
	void saveToFile(clan::IODevice& binFile);

	// ���������� ��� �������� ���������.
	std::string getGenotypeName() { return ourSpecies->getGenotypeName(); }

	// ���������� ��� ���� ���������.
	std::string getSpeciesName() { return ourSpecies->getSpeciesName(); }

	// ���������� ��������� ������ �� ������������, ���� ����� ���� � ������, ����� ����.
	bool findFreePlace(clan::Point &point);

	// ������ ���������� ����� �������� � ���������.
	// ������ ���� ������� ���������� ���������� � ���������� ������, ���� �������. ���� ����, ���������� � ����� ��������� �����������.
	bool tryLock() { return !lockFlag.test_and_set(std::memory_order_acquire); }
	// ������������� ���������� �� ��� ���, ���� �� ������� ���������� ����������.
	void lock() { for (size_t i = 0; !tryLock(); ++i) if (i % 100 == 0) std::this_thread::yield(); }
	// ������� ����������.
	void unlock() { lockFlag.clear(std::memory_order_release); }


private:
	// �������������� ��������� � ���� (������ ������ ������) � ���������� (0 - �����, 1 - ������-������, 2 - ������ � �.�. �� 7 - ������-�����).
	LocalCoord center;
	uint8_t angle;

	// ������� ����������� �������.
	int32_t vitality;

	// ���� � ����� ��������.
	DemiTime birthday;

	// ���������� ������� ������� ����.
	uint64_t ancestorsCount;

	// ��� ���������.
	std::shared_ptr<Species> ourSpecies;

	// ������ ���������.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// ��������� �� �������, ����������� �� ����, ��� ���������.
	std::shared_ptr<ChemReaction> ourReaction;

	// ������� ������ ��� �������� �������� ����� ��������.
	organismAmounts_t leftReagentAmounts;

	// ����� �����������, ����������� �� ���� ��� ���������, ���� 2 ��� �������.
	int32_t breedingReserve = 2;

	// ��� ���������� �������������� ������� �� ������ ������� (���������� � �������������).
	std::atomic_flag lockFlag;

	// ���������� �����, ������� ������������ �������� � ��������� ����������� � ������ ������������ �����������.
	void getPointAtDirection(uint8_t direction, clan::Point & dest);

	// ��������� ���������� ����� ����������.
	void decAliveCount();
};

};
