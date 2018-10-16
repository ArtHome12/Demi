/* ===============================================================================
	Моделирование эволюции живого мира.
	Модуль для живого организма.
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
// Клетка, базовый тип.
//
//GenericCell::~GenericCell(void) раскоментируем, как появится нужда.
//{
//}


//
// Организм.
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
	// Надо создать собственные клетки на основе клеток вида.
	const std::vector<std::shared_ptr<GenericCell>>& specCells = species->getCellsRef();
	for (auto &sCell : specCells) {
		GenericCell * ownCell = sCell->getClone();
		ownCell->organism = this;
		cells.push_back(std::shared_ptr<GenericCell>(ownCell));
	}

	// Разместим свои клетки в точках мира. Пока одна клетка.
	Dot &dot = center.get_dot(0, 0);
	dot.cells.push_back(cells[0]);
	dot.organism = this;

	// Откорректируем статистику.
	if (isAlive())
		ourSpecies->incAliveCount();
}



Organism::~Organism()
{
	Dot &dot = center.get_dot(0, 0);

	// Для минимизации ошибки в потоке для отображения сначала удалим ссылку на себя из точки.
	dot.organism = nullptr;

	// Пока у нас в каждой точке только одна клетка, а надо удалять адресно.
	dot.cells.pop_back();

	// Возвращаем накопленные минеральные вещества в мир.
	//
	organismAmounts_t::iterator itAmounts = leftReagentAmounts.begin();
	const auto &leftReagents = ourReaction->leftReagents;
	for (auto &reagent : leftReagents) {

		// Текущее имеющееся значение, которое надо вернуть.
		organismAmount_t amount = *itAmounts++;

		// Возвращаем.
		dot.incElemAmount(reagent.elementIndex, amount);
	}
}


bool Organism::makeTick()
{
	// Ссылка на точку, где находится клетка, для оптимизации.
	Dot& dot = center.get_dot(0, 0);

	// Необходимо проверить наличие пищи. Получим точку, где находимся.
	// Итератор на вектор количеств, чтобы синхронно двигаться с вектором вида в цикле.
	organismAmounts_t::iterator itAmounts = leftReagentAmounts.begin();
	bool isFull = true;
	for (auto &reagent : ourReaction->leftReagents) {

		// Текущее имеющееся значение в клетке.
		organismAmount_t amount = *itAmounts;

		// Необходимая величина для пополнения.
		organismAmount_t topIt = reagent.amount - amount;

		// Получаем доступное в точке количество соответствующего минерала.
		uint64_t maxAvailable = dot.getElemAmount(reagent.elementIndex);

		// Доступное количество для пополнения.
		// Если недонабрали вещества до реакции, сбросим флаг.
		if (topIt > maxAvailable) {
			topIt = organismAmount_t(maxAvailable);	// Исходя из условия здесь не будет потерь разрядов.
			isFull = false;
		}

		// Пополняем количество в клетке и вычитаем в точке.
		*itAmounts = amount + topIt;
		++itAmounts;
		dot.setElementAmount(reagent.elementIndex, maxAvailable - topIt);
	}

	// Проверка готовности к реакции - должны быть вещества и энергии.
	if (isFull && (ourReaction->geoEnergy <= dot.getGeothermalEnergy()) && (ourReaction->solarEnergy <= dot.getSolarEnergy())) {

		// Реакция прошла, выбросим в мир результаты.
		for (auto &reagent : ourReaction->rightReagents) {
			uint64_t dotAmount = dot.getElemAmount(reagent.elementIndex) + reagent.amount;
			dot.setElementAmount(reagent.elementIndex, dotAmount);

			// Не забудем обновить и общее количество.
			globalWorld.amounts.incAmount(reagent.elementIndex, reagent.amount);
		}

		// Очистим входной отсек.
		for (auto & amountItem : leftReagentAmounts)
			amountItem = 0;

		// Исходные реагенты исключим из общего количества.
		for (auto &reagent : ourReaction->leftReagents)
			globalWorld.amounts.decAmount(reagent.elementIndex, reagent.amount);

		// Сохраним полученную энергию, если не достигли максимума.
		vitality += ourReaction->vitalityProductivity;
		if (vitality > cMaxVitality)
			vitality = cMaxVitality;

		// Возвратим истину, если готовы делиться.
		return vitality >= 2;
	}
	else {
		// Реакция не прошла, вычитаем энергию на метаболизм.
		// Если жизненная энергия стала отрицательной, значит организм умер.
		// Объект сам себя уничтожить не может, он будет удалён из списка живых при первом к нему обращении.
		processInactiveVitality();

		// Возвратим ложь, у организма плохое настроение, чтобы размножаться.
		return false;
	}
}


// Вычитает жизненную энергию на неактивное состояние и обновляет статистику.
void Organism::processInactiveVitality() 
{ 
	// Был ли жив организм до вычитания.
	auto oldAlive = isAlive();

	// Расходуем энергию.
	vitality -= minActiveMetabolicRate; 

	// Если помер, то откорректируем статистику.
	if (oldAlive != isAlive())
		ourSpecies->decAliveCount(); 
}


// Возвращает свободную клетку из окрестностей, если такая есть или 0, 0.
bool Organism::findFreePlace(clan::Point &point)
{
	// Пребираем все направления, если не находим, возвращаем ложь.
	for (uint8_t i = 0; i != 8; ++i) {
		getPointAtDirection(i, point);

		if (center.get_dot(point).organism == nullptr)
			return true;
	}
	return false;
}

// Возвращает точку, лежащую относительно исходной в указанном направлении с учётом собственного направления.
void Organism::getPointAtDirection(uint8_t direction, clan::Point & dest)
{
	// Добавим к заданному направлению собственное направление.
	direction += angle;
	if (direction > 7)
		direction -= 7;

	switch (direction) {
	case 0:	// на восток.
		dest.x = 1; dest.y = 0;
		break;
	case 1: // на юго-восток.
		dest.x = 1; dest.y = 1;
		break;
	case 2:	// на юг.
		dest.x = 0; dest.y = 1;
		break;
	case 3:	// на юго-запад.
		dest.x = -1; dest.y = 1;
		break;
	case 4:	// на запад.
		dest.x = -1; dest.y = 0;
		break;
	case 5:	// на северо-запад.
		dest.x = -1; dest.y = -1;
		break;
	case 6:	// на север.
		dest.x = 0; dest.y = -1;
		break;
	default:	// на северо-восток.
		dest.x = 1; dest.y = -1;
	}
}


// Безусловное перемещение организма в другую точку.
void Organism::moveTo(const clan::Point &newCenter)
{
	// Удалим себя с прежнего местоположения.
	Dot &dot = center.get_dot(0, 0);
	dot.organism = nullptr;		// Чем раньше занулим организм, тем меньше шансов на вылет в отображающем потоке.
	auto iter = std::find(dot.cells.begin(), dot.cells.end(), cells[0]);
	if (iter != dot.cells.end())
		dot.cells.erase(iter);

	// Разместим свои клетки в точках мира. Пока одна клетка.
	center = LocalCoord(newCenter);
	Dot &newDot = center.get_dot(0, 0);
	newDot.cells.push_back(cells[0]);
	newDot.organism = this;
}

// Считывают и сохраняют себя в файл.
Organism* Organism::createFromFile(clan::File& binFile, const clan::Point& Acenter, const std::shared_ptr<Species>& Aspecies)
{
	uint8_t Aangle = binFile.read_uint8();					// angle
	int32_t Avitality = binFile.read_int32();				// vitality
	DemiTime Abirthday(binFile);							// birthday
	uint64_t AancestorsCount = binFile.read_uint64();		// ancestorsCount

	// Создаём организм.
	Organism* retVal = new Organism(Acenter, Aangle, Avitality, Abirthday, AancestorsCount, Aspecies);

	// Содержимое ячеек реакции.
	const size_t numBytes = sizeof(demi::organismAmount_t) * retVal->leftReagentAmounts.size();
	binFile.read(retVal->leftReagentAmounts.data(), numBytes);

	return retVal;
}

// Сохраняет себя в файл для последующего считывания в конструкторе.
void Organism::saveToFile(clan::File& binFile)
{
	// Center и ourSpecies не записываем, этим управляет внешний код при записи точки.
	binFile.write_uint8(angle);				// angle
	binFile.write_int32(vitality);			// vitality
	birthday.saveToFile(binFile);			// birthday
	binFile.write_uint64(ancestorsCount);	// ancestorsCount

	// Содержимое ячеек реакции.
	const size_t numBytes = sizeof(demi::organismAmount_t) * leftReagentAmounts.size();
	binFile.write(leftReagentAmounts.data(), numBytes);
}
