/* ===============================================================================
Моделирование эволюции живого мира.
Дата и время модели.
24 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "demi_time.h"

using namespace demi;

void DemiTime::MakeTick()
{
	// Прибавляет один тик.
	//
	// Прибавляем одну секунду и проверяем, не достигли ли полных суток
	if (++sec >= cTicksInDay) {

		// Обнуляем секунды, прибавляем один день и аналогично секундам.
		sec = 0;
		if (++day >= cDaysInYear) {
			day = 1;

			// Года просто прибавляем до скончания времён.
			year++;
		}
	}
}

std::string DemiTime::getDateStr() const
{
	// Возвращает строку с временем модели.
	//
	return clan::string_format("%1:%2:%3", year, day, sec);
}


// Считывают и сохраняют себя в файл для последующего считывания в конструкторе.
DemiTime* DemiTime::createFromFile(clan::File& binFile)
{
	uint16_t Asec = binFile.read_uint16();
	uint16_t Aday = binFile.read_uint16();
	uint32_t Ayear = binFile.read_uint32();

	return new DemiTime(Asec, Aday, Ayear);
}


// Сохраняет себя в файл для последующего считывания в конструкторе.
void DemiTime::saveToFile(clan::File& binFile)
{
	binFile.write_uint16(sec);
	binFile.write_uint16(day);
	binFile.write_uint32(year);
}

