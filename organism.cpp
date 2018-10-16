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
uint8_t Organism::minActiveMetabolicRate = 0;
uint8_t Organism::minInactiveMetabolicRate = 0;
int32_t Organism::desintegrationVitalityBarrier = 0;

Organism::Organism(const clan::Point &Acenter, uint8_t Aangle, int32_t Avitality, const DemiTime& Abirthday, uint64_t AancestorsCount, const std::shared_ptr<Species>& species) :
	center(Acenter),
	angle(Aangle),
	vitality(Avitality),
	birthday(Abirthday),
	ancestorsCount(AancestorsCount),
	ourSpecies(species),
	cells(), 
	ourReaction(globalWorld.getReaction(species->getGeneValueByName("Reaction"))),
	leftReagentAmounts(ourReaction->leftReagents.size())
{
	// ���� ������� ����������� ������ �� ������ ������ ����.
	const std::vector<std::shared_ptr<GenericCell>>& specCells = species->getCellsRef();
	for (auto &sCell : specCells) {
		GenericCell * ownCell = sCell->getClone();
		ownCell->organism = this;
		cells.push_back(std::shared_ptr<GenericCell>(ownCell));
	}

	// ��������� ���� ������ � ������ ����. ���� ���� ������.
	Dot &dot = center.get_dot(0, 0);
	dot.cells.push_back(cells[0]);
	dot.organism = this;

	// �������������� ����������.
	if (isAlive())
		ourSpecies->incAliveCount();
}



Organism::~Organism()
{
	Dot &dot = center.get_dot(0, 0);

	// ��� ����������� ������ � ������ ��� ����������� ������� ������ ������ �� ���� �� �����.
	dot.organism = nullptr;

	// ���� � ��� � ������ ����� ������ ���� ������, � ���� ������� �������.
	dot.cells.pop_back();

	// ���������� ����������� ����������� �������� � ���.
	//
	organismAmounts_t::iterator itAmounts = leftReagentAmounts.begin();
	const auto &leftReagents = ourReaction->leftReagents;
	for (auto &reagent : leftReagents) {

		// ������� ��������� ��������, ������� ���� �������.
		organismAmount_t amount = *itAmounts++;

		// ����������.
		dot.incElemAmount(reagent.elementIndex, amount);
	}
}


bool Organism::makeTick()
{
	// ������ �� �����, ��� ��������� ������, ��� �����������.
	Dot& dot = center.get_dot(0, 0);

	// ���������� ��������� ������� ����. ������� �����, ��� ���������.
	// �������� �� ������ ���������, ����� ��������� ��������� � �������� ���� � �����.
	organismAmounts_t::iterator itAmounts = leftReagentAmounts.begin();
	bool isFull = true;
	for (auto &reagent : ourReaction->leftReagents) {

		// ������� ��������� �������� � ������.
		organismAmount_t amount = *itAmounts;

		// ����������� �������� ��� ����������.
		organismAmount_t topIt = reagent.amount - amount;

		// �������� ��������� � ����� ���������� ���������������� ��������.
		uint64_t maxAvailable = dot.getElemAmount(reagent.elementIndex);

		// ��������� ���������� ��� ����������.
		// ���� ����������� �������� �� �������, ������� ����.
		if (topIt > maxAvailable) {
			topIt = organismAmount_t(maxAvailable);	// ������ �� ������� ����� �� ����� ������ ��������.
			isFull = false;
		}

		// ��������� ���������� � ������ � �������� � �����.
		*itAmounts = amount + topIt;
		++itAmounts;
		dot.setElementAmount(reagent.elementIndex, maxAvailable - topIt);
	}

	// �������� ���������� � ������� - ������ ���� �������� � �������.
	if (isFull && (ourReaction->geoEnergy <= dot.getGeothermalEnergy()) && (ourReaction->solarEnergy <= dot.getSolarEnergy())) {

		// ������� ������, �������� � ��� ����������.
		for (auto &reagent : ourReaction->rightReagents) {
			uint64_t dotAmount = dot.getElemAmount(reagent.elementIndex) + reagent.amount;
			dot.setElementAmount(reagent.elementIndex, dotAmount);

			// �� ������� �������� � ����� ����������.
			globalWorld.amounts.incAmount(reagent.elementIndex, reagent.amount);
		}

		// ������� ������� �����.
		for (auto & amountItem : leftReagentAmounts)
			amountItem = 0;

		// �������� �������� �������� �� ������ ����������.
		for (auto &reagent : ourReaction->leftReagents)
			globalWorld.amounts.decAmount(reagent.elementIndex, reagent.amount);

		// �������� ���������� �������, ���� �� �������� ���������.
		vitality += ourReaction->vitalityProductivity;
		if (vitality > cMaxVitality)
			vitality = cMaxVitality;

		// ��������� ������, ���� ������ ��������.
		return vitality >= 2;
	}
	else {
		// ������� �� ������, �������� ������� �� ����������.
		// ���� ��������� ������� ����� �������������, ������ �������� ����.
		// ������ ��� ���� ���������� �� �����, �� ����� ����� �� ������ ����� ��� ������ � ���� ���������.
		processInactiveVitality();

		// ��������� ����, � ��������� ������ ����������, ����� ������������.
		return false;
	}
}


// �������� ��������� ������� �� ���������� ��������� � ��������� ����������.
void Organism::processInactiveVitality() 
{ 
	// ��� �� ��� �������� �� ���������.
	auto oldAlive = isAlive();

	// ��������� �������.
	vitality -= minActiveMetabolicRate; 

	// ���� �����, �� �������������� ����������.
	if (oldAlive != isAlive())
		ourSpecies->decAliveCount(); 
}


// ���������� ��������� ������ �� ������������, ���� ����� ���� ��� 0, 0.
bool Organism::findFreePlace(clan::Point &point)
{
	// ��������� ��� �����������, ���� �� �������, ���������� ����.
	for (uint8_t i = 0; i != 8; ++i) {
		getPointAtDirection(i, point);

		if (center.get_dot(point).organism == nullptr)
			return true;
	}
	return false;
}

// ���������� �����, ������� ������������ �������� � ��������� ����������� � ������ ������������ �����������.
void Organism::getPointAtDirection(uint8_t direction, clan::Point & dest)
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
void Organism::moveTo(const clan::Point &newCenter)
{
	// ������ ���� � �������� ��������������.
	Dot &dot = center.get_dot(0, 0);
	dot.organism = nullptr;		// ��� ������ ������� ��������, ��� ������ ������ �� ����� � ������������ ������.
	auto iter = std::find(dot.cells.begin(), dot.cells.end(), cells[0]);
	if (iter != dot.cells.end())
		dot.cells.erase(iter);

	// ��������� ���� ������ � ������ ����. ���� ���� ������.
	center = LocalCoord(newCenter);
	Dot &newDot = center.get_dot(0, 0);
	newDot.cells.push_back(cells[0]);
	newDot.organism = this;
}

// ��������� � ��������� ���� � ����.
Organism* Organism::createFromFile(clan::File& binFile, const clan::Point& Acenter, const std::shared_ptr<Species>& Aspecies)
{
	uint8_t Aangle = binFile.read_uint8();					// angle
	int32_t Avitality = binFile.read_int32();				// vitality
	DemiTime Abirthday(binFile);							// birthday
	uint64_t AancestorsCount = binFile.read_uint64();		// ancestorsCount

	// ������ ��������.
	Organism* retVal = new Organism(Acenter, Aangle, Avitality, Abirthday, AancestorsCount, Aspecies);

	// ���������� ����� �������.
	const size_t numBytes = sizeof(demi::organismAmount_t) * retVal->leftReagentAmounts.size();
	binFile.read(retVal->leftReagentAmounts.data(), numBytes);

	return retVal;
}

// ��������� ���� � ���� ��� ������������ ���������� � ������������.
void Organism::saveToFile(clan::File& binFile)
{
	// Center � ourSpecies �� ����������, ���� ��������� ������� ��� ��� ������ �����.
	binFile.write_uint8(angle);				// angle
	binFile.write_int32(vitality);			// vitality
	birthday.saveToFile(binFile);			// birthday
	binFile.write_uint64(ancestorsCount);	// ancestorsCount

	// ���������� ����� �������.
	const size_t numBytes = sizeof(demi::organismAmount_t) * leftReagentAmounts.size();
	binFile.write(leftReagentAmounts.data(), numBytes);
}
