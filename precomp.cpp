/* ===============================================================================
Моделирование эволюции живого мира.
Точка поверхности.
24 august 2013.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"

// Преобразует целое число в строку с делением по разрядам.
std::string IntToStrWithDigitPlaces(unsigned long long num)
{
	std::string retVal = std::to_string(num);

	// Добавим пробелы для разделения разрядов.
	for (int i = retVal.length()-3; i > 0; i -= 3)
		retVal.insert(i, " ");

	return retVal;
}
