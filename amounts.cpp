/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для подсчёта количества элементов и организмов с целью отображения.
12 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "amounts.h"
#include "world.h"

// Инициализация класса. Его инициализация должна происходить после инициализации массивов в World, когда модель готова к первому тику.
void Amounts::Init()
{
	const size_t elemCount = globalWorld.getElemCount();

	// Выделим память под элементы.
	delete[] arResAmounts;
	arResAmounts = new unsigned long long[elemCount];

	// Очистим старые значения.
	memset(arResAmounts, 0, sizeof(unsigned long long) * elemCount);

	// Перебираем все точки и сохраняем количества.
	Dot *cur = globalWorld.getDotsArray();						// Первая точка массива.
	const clan::Size& worldSize = globalWorld.getWorldSize();
	Dot *last = cur + worldSize.width * worldSize.height;		// Точка после последней точки массива.
	while (cur < last) {

		// Суммируем ресурсы каждой точки.
		for (size_t i = 0; i != elemCount; ++i)
			arResAmounts[i] += cur->getElemAmount(i);

		// Если в точке есть организм, посчитаем ресурсы в нём.
		demi::Organism* organism = cur->organism;
		if (organism) {
			demi::organismAmounts_t::const_iterator itAmounts = organism->getLeftReagentAmounts().begin();
			const demi::ChemReaction &reaction = *organism->getChemReaction().get();
			for (auto &reagent : reaction.leftReagents) {
				// Текущее имеющееся значение во "рту" организма.
				demi::organismAmount_t amnt = *itAmounts++;
				if (amnt != 0)
					arResAmounts[reagent.elementIndex] += amnt;
			}
		}

		++cur;
	}
	
}
