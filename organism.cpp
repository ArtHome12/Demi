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
Organism::Organism(std::shared_ptr<Species> species) : ourSpecies(species), cells(species->cells)
{
	// Всем своим клеткам сообщим о себе.
	for (auto &cell : cells)
		cell->organism = this;
}


Organism::~Organism()
{
}


// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
void Organism::makeTick()
{
	// Необходимо проверить наличие пищи 
}

