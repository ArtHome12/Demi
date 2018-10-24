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
	const uint16_t cDaysInYear = 365ui16;

	// Количество тиков в сутках, приравняем к секундам 60с*60м*24ч=86400
	const uint16_t cTicksInDay = 86400ui16;
	//const uint16_t cTicksInDay = uint16_t(10);


	//
	// Формат мирового времени
	//
	class DemiTime {
	public:
		// Момент внутри дня, от 0 до cTicksInDay
		uint16_t sec = 0;

		// День года, от 1 до cDaysInYear включительно.
		uint16_t day = 1;

		// Год, от 1 до бесконечности.
		uint32_t year = 1;

		DemiTime() {};
		DemiTime(uint16_t ASec, uint16_t ADay, uint32_t AYear) : sec(ASec), day(ADay), year(AYear){};
		DemiTime(clan::IODevice& binFile);

		// Прибавляет один тик.
		void MakeTick();

		// Возвращает строку с временем.
		std::string getDateStr() const;

		const bool operator!=(const DemiTime& rv) const {
			return (year != rv.year) || (day != rv.day) || (sec != rv.sec);
		}

		// Сохраняeт себя в файл. Порядок считывания определяется порядоком объявления полей.
		void saveToFile(clan::IODevice& binFile);
	};

}