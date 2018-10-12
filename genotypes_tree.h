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

typedef std::vector<std::shared_ptr<demi::Species>> speciesDict_t;

//
// Древовидный список, в вершине - вид протоорганизма.
//
class GenotypesTree {
public:
	// Текущий генотип узла дерева.
	std::shared_ptr<demi::Genotype> genotype;

	// Список производных узлов.
	std::vector<std::shared_ptr<GenotypesTree>> derivatives;

	// Список производных видов от текущего генотипа с индексами.
	std::vector<std::shared_ptr<demi::Species>> species;

	// Очищает векторы.
	void clear() { derivatives.clear(); species.clear(); }

	// Создаёт словарь для сопоставления видов и индексов.
	void generateDict(speciesDict_t& dict);
};




