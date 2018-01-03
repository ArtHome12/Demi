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
Organism::Organism(std::shared_ptr<Species> species, const clan::Pointf &Acenter, int Aangle) : ourSpecies(species),
	cells(species->cells), 
	leftReagentAmounts(ourSpecies->reaction->leftReagents.size()),
	center(Acenter),
	angle(Aangle)
{
	// Всем своим клеткам сообщим о себе.
	for (auto &cell : cells)
		cell->organism = this;

	// Разместим свои клетки в точках мира. Пока одна клетка.
	center.get_dot(0, 0).cells.push_back(cells[0]);
}


Organism::~Organism()
{
}


// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
void Organism::makeTick()
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
		itAmounts++;
		dot.setElementAmount(reagent.elementIndex, maxAvailable - topIt);
	}

	// Проверка готовности к реакции - должны быть вещества и энергии.
	if (isFull && (reaction.geoEnergy <= dot.getGeothermalEnergy()) && (reaction.solarEnergy <= dot.getSolarEnergy())) {

		// Реакция прошла, выбросим в мир результаты.
		for (auto &reagent : reaction.rightReagents) {
			float dotAmount = dot.getElemAmount(reagent.elementIndex) + reagent.amount;
			dot.setElementAmount(reagent.elementIndex, dotAmount);
		}

		// Сохраним полученную энергию.
	}
	else {
		// Реакция не прошла, вычитаем энергию на метаболизм.
	}
}

