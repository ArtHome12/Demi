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

	//virtual ~BaseCell(void);

	// ���������� ��� ������.
	virtual CellTypes getCellType() abstract;

	// ���� ������� ������ �� ��������.
	Organism *organism = nullptr;
};


//
// ������ - �����.
class CellAbdomen : public GenericCell
{
	// ���������� ��� ������.
	virtual CellTypes getCellType() { return cellAbdomen; }
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

	void set_visible(bool AVisible) { visible = AVisible; };
	bool get_visible() { return visible; }

	// ��������� ��������� ������ ��������� � �������. ������ - ��������� ��� ���������? ���� ������ ��������� ������� �� �����,
	// ���� ����� ������ � ������������ ������. ������� ����� ������� ���������?
};

//
// ���-�������� ��� ������� �������.
//
//class Inanimal_Species_Dumb : public Species
//{
//public:
//	// ������ �������� � ������� ������� �������.
//	int inanimal_index;
//
//	Inanimal_Species_Dumb(int elementIndex) {inanimal_index = elementIndex;}
//	void set_visible(bool visible) override;
//	bool get_visible() override;
//};


//
// ��������.
//
class Organism
{
	// �������� ����� ������ ������ ������ �����.

public:
	Organism(std::shared_ptr<Species> species);
	virtual ~Organism();

	// ��� ���������.
	std::shared_ptr<Species> ourSpecies;

	// �������������� ��������� � ���� (������ ������ ������) � ���������� (0 - �����, 1 - ������-������, 2 - ������ � �.�. �� 7 - ������-�����).
	clan::Pointf center;
	int angle;

	// ������ ���������.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
	void makeTick();

	// ������������ ������� �� �������� �� ������� ������� �����.

	// ������������ ������� �� ����� �� ������� ���������.
};

};