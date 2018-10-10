/* ===============================================================================
������������� �������� ������ ����.
����������� �������� ��������� � ����� ��� �������������� � �������������.
10 october 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once

#include <vector>


//
// ����������� ������, � ������� - ��� ��������������.
//
class GenotypesTree {
public:
	// ������� ������� ���� ������.
	std::shared_ptr<demi::Genotype> genotype;

	// ������ ����������� ���������.
	std::vector<std::shared_ptr<demi::Genotype>> derivativeGenotypes;

	// ������ ����������� ����� �� �������� ��������.
	std::vector<std::shared_ptr<demi::Species>> species;

	// ������� �������.
	void clear() { derivativeGenotypes.clear(); species.clear(); }

	// ���� ���������� ��� ����� ���������, ���� ���, �� ���������, ���� ����, �� ���������� ������ �� ������������.
	const std::shared_ptr<demi::Species>& findSpecies(const std::shared_ptr<demi::Species>& speciesToFindSimilar);
};


