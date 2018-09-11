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
// Вид организма
//
// Возвращает полное название вида в формате автор/вид\автор/вид.../ Корневой общий для всех вид не включается.
//std::string Species::getFullName()
//{
//	// Если мы в корневом виде, то выходим.
//	auto spAncestor = ancestor.lock();
//	if (!spAncestor) return "";
//
//	// Если предок есть, то вернём предка и свои данные. 
//	return spAncestor->getFullName() + getAuthorAndNamePair();
//}


// Возвращает вид по указанному полному названию. Должна вызываться для корневого вида.
std::shared_ptr<Species> Species::getSpeciesByFullName(std::string fullName)
{
	return nullptr;
}



//
// Организм.
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
	// Надо создать собственные клетки на основе клеток вида.
	for (auto &sCell : species->cells) {
		GenericCell * ownCell = sCell->getClone();
		ownCell->organism = this;
		cells.push_back(std::shared_ptr<GenericCell>(ownCell));
	}

	// Разместим свои клетки в точках мира. Пока одна клетка.
	Dot &dot = center.get_dot(0, 0);
	dot.cells.push_back(cells[0]);
	dot.organism = this;
}


Organism::~Organism()
{
	// Удаляем собственные клетки из точек.
	Dot &dot = center.get_dot(0, 0);

	// Возвращаем накопленные минеральные вещества в мир.
	//
	std::vector<float>::iterator itAmounts = leftReagentAmounts.begin();
	const demi::ChemReaction &reaction = *ourSpecies->reaction;
	for (auto &reagent : reaction.leftReagents) {

		// Текущее имеющееся значение, которое надо вернуть.
		float amount = *itAmounts++;

		// Получаем доступное в точке количество соответствующего минерала.
		float amountInDot = dot.getElemAmount(reagent.elementIndex);

		dot.setElementAmount(reagent.elementIndex, amountInDot + amount);
	}

	// Пока у нас в каждой точке только одна клетка, а надо удалять адресно.
	dot.cells.pop_back();
	dot.organism = nullptr;
}


// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
Organism* Organism::makeTickAndGetNewBorn()
{
	// Ссылка на точку, где находится клетка, для оптимизации.
	Dot& dot = center.get_dot(0, 0);

	// Необходимо проверить наличие пищи. Получим точку, где находимся.
	// Итератор на вектор количеств, чтобы синхронно двигаться с вектором вида в цикле.
	std::vector<float>::iterator itAmounts = leftReagentAmounts.begin();
	bool isFull = true;
	const demi::ChemReaction &reaction = *ourSpecies->reaction;
	for (auto &reagent : reaction.leftReagents) {

		// Текущее имеющееся значение.
		float amount = *itAmounts;

		// Величина для пополнения.
		float topIt = reagent.amount - amount;

		// Получаем доступное в точке количество соответствующего минерала.
		float maxAvailable = dot.getElemAmount(reagent.elementIndex);

		// Доступное количество для пополнения.
		// Если недонабрали вещества до реакции, сбросим флаг.
		if (topIt > maxAvailable) {
			topIt = maxAvailable;
			isFull = false;
		}

		// Пополняем количество в клетке и вычитаем в точке.
		*itAmounts = amount + topIt;
		++itAmounts;
		dot.setElementAmount(reagent.elementIndex, maxAvailable - topIt);
	}

	// Проверка готовности к реакции - должны быть вещества и энергии.
	if (isFull && (reaction.geoEnergy <= dot.getGeothermalEnergy()) && (reaction.solarEnergy <= dot.getSolarEnergy())) {

		// Реакция прошла, выбросим в мир результаты.
		for (auto &reagent : reaction.rightReagents) {
			float dotAmount = dot.getElemAmount(reagent.elementIndex) + reagent.amount;
			dot.setElementAmount(reagent.elementIndex, dotAmount);
		}

		// Очистим входной отсек.
		for (auto & amountItem : leftReagentAmounts)
			amountItem = 0;

		// Сохраним полученную энергию.
		vitality += reaction.vitalityProductivity;

		// Самое время делиться, если есть такая возможность.
		clan::Pointf freePlace;
		if (vitality >= fissionBarrier && findFreePlace(freePlace)) {
			// Создаём новый экземпляр, передаём ему половину энергии, на порог деления делаем мутацию в пределах 1%.
			vitality /= 2;
			std::uniform_int_distribution<> rndAngle(0, 7);
			int childAngle = rndAngle(globalWorld.getRandomGenerator());
			std::uniform_real_distribution<float> rndFission(0.99f, 1.01f);
			float childFissionBarrier = fissionBarrier * rndFission(globalWorld.getRandomGenerator());
			return new Organism(get_species(), center.getGlobalPoint(freePlace), childAngle, vitality, childFissionBarrier);
		}
	}
	else {
		// Реакция не прошла, вычитаем энергию на метаболизм.
		vitality -= minInactiveMetabolicRate;

		// Если жизненная энергия стала отрицательной, значит организм умер.
		// Объект сам себя уничтожить не может, он будет удалён из списка живых при первом к нему обращении.
	}

	return nullptr;
}


// Возвращает свободную клетку из окрестностей, если такая есть или 0, 0.
bool Organism::findFreePlace(clan::Pointf &point)
{
	// Пребираем все направления, если не находим, возвращаем ложь.
	for (int i = 0; i <= 7; i++) {
		getPointAtDirection(i, point);

		if (center.get_dot(point).organism == nullptr)
			return true;
	}
	return false;
}

// Возвращает точку, лежащую относительно исходной в указанном направлении с учётом собственного направления.
void Organism::getPointAtDirection(int direction, clan::Pointf & dest)
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
void Organism::moveTo(const clan::Pointf &newCenter)
{
	// Удалим себя с прежнего местоположения.
	Dot &dot = center.get_dot(0, 0);
	auto iter = std::find(dot.cells.begin(), dot.cells.end(), cells[0]);
	if (iter != dot.cells.end())
		dot.cells.erase(iter);
	dot.organism = nullptr;

	// Разместим свои клетки в точках мира. Пока одна клетка.
	center = LocalCoord(newCenter);
	Dot &newDot = center.get_dot(0, 0);
	newDot.cells.push_back(cells[0]);
	newDot.organism = this;
}
