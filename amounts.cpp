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
	const int elemCount = globalWorld.getElemCount();

	// Выделим память под элементы.
	delete[] arResAmounts;
	arResAmounts = new long double[elemCount];

	// Для борьбы с проблемой потери значимости значения меньше единицы складываем отдельно от значений больше единицы.
	// Локальный массив для значений меньше 1.
	long double* arTinyAmounts = new long double[elemCount];

	// Очистим.
	for (int i = 0; i < elemCount; ++i) 
		arTinyAmounts[i] = arResAmounts[i] = 0;
	

	try {
		// Перебираем все точки и сохраняем количества.
		Dot *cur = globalWorld.getDotsArray();						// Первая точка массива.
		clan::Sizef worldSize = globalWorld.get_worldSize();
		Dot *last = cur + int(worldSize.width * worldSize.height);	// Точка после последней точки массива.
		while (cur < last) {

			// В каждой точке перебираем все ресурсы.
			for (int i = 0; i < elemCount; ++i) {
			
				double amnt = cur->getElemAmount(i);

				// Маленькие величины складываем отдельно, незначительная оптимизация.
				if (amnt < 1.0)
					arTinyAmounts[i] += amnt;
				else
					arResAmounts[i] += amnt;
			}

			// Если в точке есть организм, посчитаем ресурсы в нём.
			demi::Organism* organism = cur->organism;
			if (organism) {

				std::vector<float>::iterator itAmounts = organism->leftReagentAmounts.begin();
				const demi::ChemReaction &reaction = *organism->get_species()->reaction;
				for (auto &reagent : reaction.leftReagents) {

					// Текущее имеющееся значение во "рту" организма.
					float amnt = *itAmounts++;

					// Откорректируем сумму в общем количестве.
					if (amnt < 1.0)
						arTinyAmounts[reagent.elementIndex] += amnt;
					else
						arResAmounts[reagent.elementIndex] += amnt;
				}

			}

			++cur;
		}

		// Добавим малое к большому.
		for (int i = 0; i < elemCount; ++i)
			arResAmounts[i] += arTinyAmounts[i];

		delete[] arTinyAmounts;
	
	} catch (...) {
		// В случае исключения освободим память и продолжим.
		delete[] arTinyAmounts;
		throw;
	}
}
