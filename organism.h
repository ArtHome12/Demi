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
class GenericCell
{
public:
	// �������������� ������ � ������� ��������� ���������.
	int x, y;

	//virtual ~BaseCell(void);
};


//
// ��������.
//
class Organism
{
	// �������� ����� ������ ������ ������ �����.

public:
	Organism(void);
	virtual ~Organism(void);
};

};