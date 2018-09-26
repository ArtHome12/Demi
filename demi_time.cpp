/* ===============================================================================
������������� �������� ������ ����.
���� � ����� ������.
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
	// ���������� ���� ���.
	//
	// ���������� ���� ������� � ���������, �� �������� �� ������ �����
	if (++sec >= cTicksInDay) {

		// �������� �������, ���������� ���� ���� � ���������� ��������.
		sec = 0;
		if (++day >= cDaysInYear) {
			day = 1;

			// ���� ������ ���������� �� ��������� �����.
			year++;
		}
	}
}

std::string DemiTime::getDateStr() const
{
	// ���������� ������ � �������� ������.
	//
	return clan::string_format("%1:%2:%3", year, day, sec);
}


// ��������� � ��������� ���� � ���� ��� ������������ ���������� � ������������.
DemiTime* DemiTime::createFromFile(clan::File& binFile)
{
	uint16_t Asec = binFile.read_uint16();
	uint16_t Aday = binFile.read_uint16();
	uint32_t Ayear = binFile.read_uint32();

	return new DemiTime(Asec, Aday, Ayear);
}


// ��������� ���� � ���� ��� ������������ ���������� � ������������.
void DemiTime::saveToFile(clan::File& binFile)
{
	binFile.write_uint16(sec);
	binFile.write_uint16(day);
	binFile.write_uint32(year);
}

