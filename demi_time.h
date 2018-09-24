/* ===============================================================================
Моделирование эволюции живого мира.
Дата и время модели.
24 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once


namespace demi {


	// Продолжительность года в днях.
	const size_t cDaysInYear = 365;

	// Количество тиков в сутках, приравняем к секундам 60с*60м*24ч=86400
	const size_t cTicksInDay = 86400;
	//const int cTicksInDay = 10;


	//
	// Формат мирового времени
	//
	class DemiTime {
	public:
		// Год, от 1 до бесконечности.
		size_t year = 1;

		// День года, от 1 до cDaysInYear включительно.
		size_t day = 1;

		// Момент внутри дня, от 0 до cTicksInDay
		size_t sec = 0;

		// Прибавляет один тик.
		void MakeTick();

		// Возвращает строку с временем.
		std::string getDateStr() const;

		const bool operator!=(const DemiTime& rv) const {
			return (year != rv.year) || (day != rv.day) || (sec != rv.sec);
		}
	};

}