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
Organism::Organism(std::shared_ptr<Species> species, const clan::Pointf &Acenter, int Aangle) : ourSpecies(species),
	cells(species->cells), 
	leftReagentAmounts(ourSpecies->reaction->leftReagents.size()),
	center(Acenter),
	angle(Aangle)
{
	// ���� ����� ������� ������� � ����.
	for (auto &cell : cells)
		cell->organism = this;

	// ��������� ���� ������ � ������ ����. ���� ���� ������.
	center.get_dot(0, 0).cells.push_back(cells[0]);
}


Organism::~Organism()
{
}


// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
void Organism::makeTick()
{
	// ������ �� �����, ��� ��������� ������, ��� �����������.
	Dot& dot = center.get_dot(0, 0);

	// ���������� ��������� ������� ����. ������� �����, ��� ���������.
	// �������� �� ������ ���������, ����� ��������� ��������� � �������� ���� � �����.
	std::vector<float>::iterator itAmounts = leftReagentAmounts.begin();
	bool isFull = true;
	const demi::ChemReaction &reaction = *ourSpecies->reaction;
	for (auto &reagent : reaction.leftReagents) {

		// ������� ��������� ��������.
		float amount = *itAmounts;

		// �������� ��� ����������.
		float topIt = reagent.amount - amount;

		// �������� ��������� � ����� ���������� ���������������� ��������.
		float maxAvailable = dot.getElemAmount(reagent.elementIndex);

		// ��������� ���������� ��� ����������.
		// ���� ����������� �������� �� �������, ������� ����.
		if (topIt > maxAvailable) {
			topIt = maxAvailable;
			isFull = false;
		}

		// ��������� ���������� � ������ � �������� � �����.
		*itAmounts = amount + topIt;
		itAmounts++;
		dot.setElementAmount(reagent.elementIndex, maxAvailable - topIt);
	}

	// �������� ���������� � ������� - ������ ���� �������� � �������.
	if (isFull && (reaction.geoEnergy <= dot.getGeothermalEnergy()) && (reaction.solarEnergy <= dot.getSolarEnergy())) {

		// ������� ������, �������� � ��� ����������.
		for (auto &reagent : reaction.rightReagents) {
			float dotAmount = dot.getElemAmount(reagent.elementIndex) + reagent.amount;
			dot.setElementAmount(reagent.elementIndex, dotAmount);
		}

		// �������� ���������� �������.
	}
	else {
		// ������� �� ������, �������� ������� �� ����������.
	}
}

