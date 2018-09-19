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



namespace demi {

	// ���� ������ - ����, ��������, �����, ���, �����, ���, �����.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

//
// ������, ������� ���.
//
class Organism;
class GenericCell
{
public:
	// �������������� ������ � ������� ��������� ���������.
	int x, y;

	GenericCell() {}
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
	int fissionBarrier;

	void set_visible(bool AVisible) { visible = AVisible; };
	bool get_visible() { return visible; }

	// ���������� ������ �������� ���� � ������� �����/���\�����/���... �������� ����� ��� ���� ��� �� ����������.
	std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
	//std::string getFullName();

	// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
	std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);
};


//
// ��������.
//
class Organism
{
public:
	Organism(std::shared_ptr<Species> species, const clan::Point &Acenter, int Aangle, int Avitality, int AfissionBarrier);
	~Organism();

	// �������������� ��������� � ���� (������ ������ ������) � ���������� (0 - �����, 1 - ������-������, 2 - ������ � �.�. �� 7 - ������-�����).
	int angle;
	LocalCoord center;

	// ������ ���������.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// ������ � �����.
	std::shared_ptr<Species> get_species() { return ourSpecies; }

	// ����������� ������� ����������� ��� �������� ������ � ��� ���������.
	static int minActiveMetabolicRate, minInactiveMetabolicRate;

	// ��������� �������, ���� ������� �������� �����������.
	static int desintegrationVitalityBarrier;

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

	int getVitality() { return vitality; }
	int getFissionBarrier() { return fissionBarrier; }

	// �������� ��������� ������� �� ���������� ���������.
	void processInactiveVitality() { vitality -= minActiveMetabolicRate; }

private:
	// ��� ���������.
	std::shared_ptr<Species> ourSpecies;

public:
	// ������� ������ ��� �������� �������� ����� ��������.
	std::vector<unsigned long long> leftReagentAmounts;

private:
	// ������� ����������� �������.
	int vitality;

	// ����� ����������� ��� ��������� (���������� ��������� �� ��������� ��� ����, ����� �������� ��-�� ������������).
	int fissionBarrier;

	// ���������� ��������� ������ �� ������������, ���� ����� ���� � ������, ����� ����.
	bool findFreePlace(clan::Point &point);

	// ���������� �����, ������� ������������ �������� � ��������� ����������� � ������ ������������ �����������.
	void getPointAtDirection(int direction, clan::Point & dest);
};

};