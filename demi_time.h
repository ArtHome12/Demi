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
	const size_t cDaysInYear = 365;

	// ���������� ����� � ������, ���������� � �������� 60�*60�*24�=86400
	const size_t cTicksInDay = 86400;
	//const int cTicksInDay = 10;


	//
	// ������ �������� �������
	//
	class DemiTime {
	public:
		// ���, �� 1 �� �������������.
		size_t year = 1;

		// ���� ����, �� 1 �� cDaysInYear ������������.
		size_t day = 1;

		// ������ ������ ���, �� 0 �� cTicksInDay
		size_t sec = 0;

		// ���������� ���� ���.
		void MakeTick();

		// ���������� ������ � ��������.
		std::string getDateStr() const;

		const bool operator!=(const DemiTime& rv) const {
			return (year != rv.year) || (day != rv.day) || (sec != rv.sec);
		}
	};

}