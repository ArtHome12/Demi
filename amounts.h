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

	unsigned long long getResAmounts(size_t index) { return arResAmounts ? arResAmounts[index] : 0; }

	// ����������� ���������� ���������� ��������. ����� ����������� �� ������������� ��������.
	void incAmount(size_t index, unsigned long long amnt) { arResAmounts[index] += amnt; }
	void decAmount(size_t index, unsigned long long amnt) { arResAmounts[index] -= amnt; }

private:

	// ������ ��������� ��������� ������� �������. 
	unsigned long long *arResAmounts = nullptr;

};