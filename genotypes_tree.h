/* ===============================================================================
Моделирование эволюции живого мира.
Древовидная иерархия генотипов и видов для взаимодействия с пользователем.
10 october 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once

#include <vector>


//
// Древовидный список, в вершине - вид протоорганизма.
//
class GenotypesTree {
public:
	// Текущий генотип узла дерева.
	std::shared_ptr<demi::Genotype> genotype;

	// Список производных генотипов.
	std::vector<std::shared_ptr<demi::Genotype>> derivativeGenotypes;

	// Список производных видов от текущего генотипа.
	std::vector<std::shared_ptr<demi::Species>> species;

	// Очищает векторы.
	void clear() { derivativeGenotypes.clear(); species.clear(); }

	// Ищет переданный вид среди имеющихся, если нет, то добавляет, если есть, то возвращает ссылку на существующий.
	const std::shared_ptr<demi::Species>& findSpecies(const std::shared_ptr<demi::Species>& speciesToFindSimilar);
};


