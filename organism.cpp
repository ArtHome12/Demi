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
// ��� ���������
//
// ���������� ������ �������� ���� � ������� �����/���\�����/���.../ �������� ����� ��� ���� ��� �� ����������.
//std::string Species::getFullName()
//{
//	// ���� �� � �������� ����, �� �������.
//	auto spAncestor = ancestor.lock();
//	if (!spAncestor) return "";
//
//	// ���� ������ ����, �� ����� ������ � ���� ������. 
//	return spAncestor->getFullName() + getAuthorAndNamePair();
//}


// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
std::shared_ptr<Species> Species::getSpeciesByFullName(std::string fullName)
{
	return nullptr;
}



//
// ��������.
//
float Organism::minActiveMetabolicRate = 0;
float Organism::minInactiveMetabolicRate = 0;
float Organism::desintegrationVitalityBarrier = 0;

Organism::Organism(std::shared_ptr<Species> species, const clan::Pointf &Acenter, int Aangle, float Avitality, float AfissionBarrier) : ourSpecies(species),
	cells(), 
	leftReagentAmounts(ourSpecies->reaction->leftReagents.size()),
	center(Acenter),
	angle(Aangle),
	vitality(Avitality), fissionBarrier(AfissionBarrier)
{
	// ���� ������� ����������� ������ �� ������ ������ ����.
	for (auto &sCell : species->cells) {
		GenericCell * ownCell = sCell->getClone();
		ownCell->organism = this;
		cells.push_back(std::shared_ptr<GenericCell>(ownCell));
	}

	// ��������� ���� ������ � ������ ����. ���� ���� ������.
	Dot &dot = center.get_dot(0, 0);
	dot.cells.push_back(cells[0]);
	dot.organism = this;
}


Organism::~Organism()
{
	// ������� ����������� ������ �� �����.
	Dot &dot = center.get_dot(0, 0);

	// ���������� ����������� ����������� �������� � ���.
	//
	std::vector<float>::iterator itAmounts = leftReagentAmounts.begin();
	const demi::ChemReaction &reaction = *ourSpecies->reaction;
	for (auto &reagent : reaction.leftReagents) {

		// ������� ��������� ��������, ������� ���� �������.
		float amount = *itAmounts++;

		// �������� ��������� � ����� ���������� ���������������� ��������.
		float amountInDot = dot.getElemAmount(reagent.elementIndex);

		dot.setElementAmount(reagent.elementIndex, amountInDot + amount);
	}

	// ���� � ��� � ������ ����� ������ ���� ������, � ���� ������� �������.
	dot.cells.pop_back();
	dot.organism = nullptr;
}


// ������������ ����� ��������� ��� ������������ ��������� - �������� ����, �����, ���������, �����������, �����������.
Organism* Organism::makeTickAndGetNewBorn()
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
		++itAmounts;
		dot.setElementAmount(reagent.elementIndex, maxAvailable - topIt);
	}

	// �������� ���������� � ������� - ������ ���� �������� � �������.
	if (isFull && (reaction.geoEnergy <= dot.getGeothermalEnergy()) && (reaction.solarEnergy <= dot.getSolarEnergy())) {

		// ������� ������, �������� � ��� ����������.
		for (auto &reagent : reaction.rightReagents) {
			float dotAmount = dot.getElemAmount(reagent.elementIndex) + reagent.amount;
			dot.setElementAmount(reagent.elementIndex, dotAmount);
		}

		// ������� ������� �����.
		for (auto & amountItem : leftReagentAmounts)
			amountItem = 0;

		// �������� ���������� �������.
		vitality += reaction.vitalityProductivity;

		// ����� ����� ��������, ���� ���� ����� �����������.
		clan::Pointf freePlace;
		if (vitality >= fissionBarrier && findFreePlace(freePlace)) {
			// ������ ����� ���������, ������� ��� �������� �������, �� ����� ������� ������ ������� � �������� 1%.
			vitality /= 2;
			std::uniform_int_distribution<> rndAngle(0, 7);
			int childAngle = rndAngle(globalWorld.getRandomGenerator());
			std::uniform_real_distribution<float> rndFission(0.99f, 1.01f);
			float childFissionBarrier = fissionBarrier * rndFission(globalWorld.getRandomGenerator());
			return new Organism(get_species(), center.getGlobalPoint(freePlace), childAngle, vitality, childFissionBarrier);
		}
	}
	else {
		// ������� �� ������, �������� ������� �� ����������.
		vitality -= minInactiveMetabolicRate;

		// ���� ��������� ������� ����� �������������, ������ �������� ����.
		// ������ ��� ���� ���������� �� �����, �� ����� ����� �� ������ ����� ��� ������ � ���� ���������.
	}

	return nullptr;
}


// ���������� ��������� ������ �� ������������, ���� ����� ���� ��� 0, 0.
bool Organism::findFreePlace(clan::Pointf &point)
{
	// ��������� ��� �����������, ���� �� �������, ���������� ����.
	for (int i = 0; i <= 7; i++) {
		getPointAtDirection(i, point);

		if (center.get_dot(point).organism == nullptr)
			return true;
	}
	return false;
}

// ���������� �����, ������� ������������ �������� � ��������� ����������� � ������ ������������ �����������.
void Organism::getPointAtDirection(int direction, clan::Pointf & dest)
{
	// ������� � ��������� ����������� ����������� �����������.
	direction += angle;
	if (direction > 7)
		direction -= 7;

	switch (direction) {
	case 0:	// �� ������.
		dest.x = 1; dest.y = 0;
		break;
	case 1: // �� ���-������.
		dest.x = 1; dest.y = 1;
		break;
	case 2:	// �� ��.
		dest.x = 0; dest.y = 1;
		break;
	case 3:	// �� ���-�����.
		dest.x = -1; dest.y = 1;
		break;
	case 4:	// �� �����.
		dest.x = -1; dest.y = 0;
		break;
	case 5:	// �� ������-�����.
		dest.x = -1; dest.y = -1;
		break;
	case 6:	// �� �����.
		dest.x = 0; dest.y = -1;
		break;
	default:	// �� ������-������.
		dest.x = 1; dest.y = -1;
	}

}


// ����������� ����������� ��������� � ������ �����.
void Organism::moveTo(const clan::Pointf &newCenter)
{
	// ������ ���� � �������� ��������������.
	Dot &dot = center.get_dot(0, 0);
	auto iter = std::find(dot.cells.begin(), dot.cells.end(), cells[0]);
	if (iter != dot.cells.end())
		dot.cells.erase(iter);
	dot.organism = nullptr;

	// ��������� ���� ������ � ������ ����. ���� ���� ������.
	center = LocalCoord(newCenter);
	Dot &newDot = center.get_dot(0, 0);
	newDot.cells.push_back(cells[0]);
	newDot.organism = this;
}
