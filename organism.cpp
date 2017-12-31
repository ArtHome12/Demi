/* ===============================================================================
	������������� �������� ������ ����.
	������ ��� ������ ���������.
	10 april 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "reactions.h"
#include "organism.h"
#include "world.h"

using namespace demi;

//
// ������, ������� ���.
//
//GenericCell::~GenericCell(void) ��������������, ��� �������� �����.
//{
//}



//
// ��������.
//
Organism::Organism(std::shared_ptr<Species> species) : ourSpecies(species), cells(species->cells)
{
	// ���� ����� ������� ������� � ����.
	for (auto &cell : cells)
		cell->organism = this;
}


Organism::~Organism()
{
}


// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
void Organism::makeTick()
{
	// ���������� ��������� ������� ���� 
}

