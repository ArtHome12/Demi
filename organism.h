/* ===============================================================================
	Моделирование эволюции живого мира.
	Модуль для живого организма.
	10 april 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once



namespace demi {

	// Типы клеток - мозг, рецептор, мышца, жир, живот, рот, броня.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

//
// Клетка, базовый тип.
//
class GenericCell
{
public:
	// Местоположение клетки в системе координат организма.
	int x, y;

	//virtual ~BaseCell(void);
};


//
// Организм.
//
class Organism
{
	// Организм имеет списки клеток разных типов.

public:
	Organism(void);
	virtual ~Organism(void);
};

};