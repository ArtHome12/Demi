/* ===============================================================================
������������� �������� ������ ����.
������ ��� �������� ���������� ��������� � ���������� � ����� �����������.
12 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

class Amounts {

public:

	~Amounts() { delete[] arResAmounts; }

	// ������������� ������. ��� ������������� ������ ����������� ����� ������������� �������� � World, ����� ������ ������ � ������� ����.
	void Init();

	double getResAmounts(int index) { return arResAmounts[index]; }

	// ����������� ���������� ���������� ��������. ����� ����������� �� ������������� ��������.
	void incAmount(int index, double amnt) { arResAmounts[index] += amnt; }
	void decAmount(int index, double amnt) { arResAmounts[index] -= amnt; }

private:

	// ������ ��������� ��������� ������� �������. 
	long double *arResAmounts = nullptr;

};