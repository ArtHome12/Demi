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

#include "reactions.h"
#include "local_coord.h"
#include "demi_time.h"


namespace demi {

	// ���� ������ - ����, ��������, �����, ���, �����, ���, �����.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

	// ��� ��� �������� ������� ����������� ������� � ���������.
	typedef uint8_t organismAmount_t;
	typedef std::vector<organismAmount_t> organismAmounts_t;

	const uint16_t cMaxVitality = UINT16_MAX - UINT8_MAX;
//
// ������, ������� ���.
//
	class Organism;
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
// ��� ���������
//
class Species
{
public:
	Species(const std::weak_ptr<Species>& Aancestor,
		const std::string& Aname,
		const std::string& Aauthor,
		bool Avisible,
		uint16_t AfissionBarrier,
		const clan::Color& AaliveColor,
		const clan::Color& AdeadColor,
		const std::shared_ptr<ChemReaction>& Areaction
		);

	// ������ � �����.
	const std::weak_ptr<Species>& getAncestor() { return ancestor; }
	const std::string& getName() { return name; }
	const std::string& getAuthor() { return author; }
	void setVisible(bool AVisible) { visible = AVisible; };
	bool getVisible() { return visible; }
	const clan::Color& getAliveColor() { return aliveColor; }
	const clan::Color& getDeadColor() { return deadColor; }
	uint16_t getFissionBarrier() { return fissionBarrier; }
	const std::shared_ptr<ChemReaction>& getReaction() { return reaction; }

	// ���������� ������������� ������ ��� ��������� ������ ��� ����������� ��������������.
	std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

	// ���������� ������ �������� ���� � ������� �����/���\�����/���... �������� ����� ��� ���� ��� �� ����������.
	std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
	//std::string getFullName();

	// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
	std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);

	// ��� ������� ���������� ���������� �� ���������� �����.
	void incAliveCount() { ++aliveCount; }
	void decAliveCount() { --aliveCount; }
	size_t getAliveCount() const { return aliveCount; }

private:
	// ������������ ��������.
	std::weak_ptr<Species> ancestor;

	// �������� ���������. ������ ������ ��������� ��� ����� �� ��� �����������, �� ���������� ����������� ���������
	// ���������� ��� ������ ������ �� ����, ������� ���������� � �����, ��-��������� ������������, �������� ����� ���.
	// ���� �������� ����� ������� ������������ ��������� ��� �������� ���������� � ������ - ������� Species, ��� ����,
	// � ������ ����� ������.
	std::vector<std::shared_ptr<Species>> descendants;

	// ������ ���������.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// �������� ���������.
	std::string name;

	// �����
	std::string author;

	// ��������� ����.
	bool visible;

	// ���� ��� ������ � ������� ����������.
	clan::Color aliveColor, deadColor;

	// ���������������� ������� ���������.
	std::shared_ptr<ChemReaction> reaction;

	// ��������� ����� ����������� (����� �������� ��-�� ������������).
	uint16_t fissionBarrier;

	// ���������� ����� ���������� ������� ���� (��� ����������).
	size_t aliveCount = 0;
};


//
// ��������.
//
class Organism
{
public:
	Organism(const clan::Point &Acenter, uint8_t Aangle, uint16_t AfissionBarrier, int32_t Avitality, const DemiTime& Abirthday, uint64_t AancestorsCount, const std::shared_ptr<Species>& species);
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
	uint16_t getFissionBarrier() const { return fissionBarrier; }


	// ����������� ������� ����������� ��� �������� ������ � ��� ���������.
	static uint8_t minActiveMetabolicRate, minInactiveMetabolicRate;

	// ��������� �������, ���� ������� �������� �����������.
	static int32_t desintegrationVitalityBarrier;

	// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
	// ����� ������� ��������� �� ����� ���������� ��������.
	Organism* makeTickAndGetNewBorn();

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
	static Organism* createFromFile(clan::File& binFile, const clan::Point& Acenter, const std::shared_ptr<Species>& Aspecies);
	void saveToFile(clan::File& binFile);

private:
	// �������������� ��������� � ���� (������ ������ ������) � ���������� (0 - �����, 1 - ������-������, 2 - ������ � �.�. �� 7 - ������-�����).
	LocalCoord center;
	uint8_t angle;

	// ����� ����������� ��� ��������� (���������� ��������� �� ��������� ��� ����, ����� �������� ��-�� ������������).
	uint16_t fissionBarrier;

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

	// ������� ������ ��� �������� �������� ����� ��������.
	organismAmounts_t leftReagentAmounts;

	// ���������� ��������� ������ �� ������������, ���� ����� ���� � ������, ����� ����.
	bool findFreePlace(clan::Point &point);

	// ���������� �����, ������� ������������ �������� � ��������� ����������� � ������ ������������ �����������.
	void getPointAtDirection(uint8_t direction, clan::Point & dest);
};

};