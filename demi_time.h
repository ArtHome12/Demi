/* ===============================================================================
������������� �������� ������ ����.
���� � ����� ������.
24 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once


namespace demi {


	// ����������������� ���� � ����.
	const uint16_t cDaysInYear = 365ui16;

	// ���������� ����� � ������, ���������� � �������� 60�*60�*24�=86400
	const uint16_t cTicksInDay = 86400ui16;
	//const uint16_t cTicksInDay = uint16_t(10);


	//
	// ������ �������� �������
	//
	class DemiTime {
	public:
		// ������ ������ ���, �� 0 �� cTicksInDay
		uint16_t sec = 0;

		// ���� ����, �� 1 �� cDaysInYear ������������.
		uint16_t day = 1;

		// ���, �� 1 �� �������������.
		uint32_t year = 1;

		DemiTime() {};
		DemiTime(uint16_t ASec, uint16_t ADay, uint32_t AYear) : sec(ASec), day(ADay), year(AYear){};
		DemiTime(clan::IODevice& binFile);

		// ���������� ���� ���.
		void MakeTick();

		// ���������� ������ � ��������.
		std::string getDateStr() const;

		const bool operator!=(const DemiTime& rv) const {
			return (year != rv.year) || (day != rv.day) || (sec != rv.sec);
		}

		// �������e� ���� � ����. ������� ���������� ������������ ��������� ���������� �����.
		void saveToFile(clan::IODevice& binFile);
	};

}