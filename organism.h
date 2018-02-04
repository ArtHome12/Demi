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
	clan::Colorf aliveColor, deadColor;

	// ���������������� ������� ���������.
	std::shared_ptr<ChemReaction> reaction;

	// ��������� ����� ����������� (����� �������� ��-�� ������������).
	float fissionBarrier;

	void set_visible(bool AVisible) { visible = AVisible; };
	bool get_visible() { return visible; }

	// ���������� ������ �������� ���� � ������� �����/���\�����/���... �������� ����� ��� ���� ��� �� ����������.
	std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
	std::string getFullName();

	// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
	Species *getSpeciesByFullName(std::string fullName);
};


//
// ��������.
//
class Organism
{
public:
	Organism(std::shared_ptr<Species> species, const clan::Pointf &Acenter, int Aangle, float Avitality, float AfissionBarrier);
	~Organism();

	// �������������� ��������� � ���� (������ ������ ������) � ���������� (0 - �����, 1 - ������-������, 2 - ������ � �.�. �� 7 - ������-�����).
	int angle;
	LocalCoord center;

	// ������ ���������.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// ������ � �����.
	std::shared_ptr<Species> get_species() { return ourSpecies; }

	// ����������� ������� ����������� ��� �������� ������ � ��� ���������.
	static float minActiveMetabolicRate, minInactiveMetabolicRate;

	// ��������� �������, ���� ������� �������� �����������.
	static float desintegrationVitalityBarrier;

	// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
	// ����� ������� ��������� �� ����� ���������� ��������.
	Organism* makeTickAndGetNewBorn();

	// ������������ ������� �� �������� �� ������� ������� �����, ���������� ������, ���� ����� �������������.
	bool canMove() { return true; }

	// ������������ ������� �� ����� �� ������� ���������.

	// ����������� ����������� ��������� � ������ �����.
	void moveTo(const clan::Pointf &newCenter);

	// ������, ���� �������� ���. ���� ���, �� ��� ���� ��������� �� ������ �����.
	bool isAlive() { return vitality > 0; }

	// ������, ���� �������� ��� ���������� � ��� ���� ����������.
	bool needDesintegration() { return vitality < desintegrationVitalityBarrier; }

	float getVitality() { return vitality; }
	float getFissionBarrier() { return fissionBarrier; }

	// �������� ��������� ������� �� ���������� ���������.
	void processInactiveVitality() { vitality -= minActiveMetabolicRate; }

private:
	// ��� ���������.
	std::shared_ptr<Species> ourSpecies;

public:
	// ������� ������ ��� �������� �������� ����� ��������.
	std::vector<float> leftReagentAmounts;

private:
	// ������� ����������� �������.
	float vitality;

	// ����� ����������� ��� ��������� (���������� ��������� �� ��������� ��� ����, ����� �������� ��-�� ������������).
	float fissionBarrier;

	// ���������� ��������� ������ �� ������������, ���� ����� ���� � ������, ����� ����.
	bool findFreePlace(clan::Pointf &point);

	// ���������� �����, ������� ������������ �������� � ��������� ����������� � ������ ������������ �����������.
	void getPointAtDirection(int direction, clan::Pointf & dest);
};

};