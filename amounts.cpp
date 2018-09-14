/* ===============================================================================
������������� �������� ������ ����.
������ ��� �������� ���������� ��������� � ���������� � ����� �����������.
12 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "amounts.h"
#include "world.h"

// ������������� ������. ��� ������������� ������ ����������� ����� ������������� �������� � World, ����� ������ ������ � ������� ����.
void Amounts::Init()
{
	const int elemCount = globalWorld.getElemCount();

	// ������� ������ ��� ��������.
	delete[] arResAmounts;
	arResAmounts = new long double[elemCount];

	// ��� ������ � ��������� ������ ���������� �������� ������ ������� ���������� �������� �� �������� ������ �������.
	// ��������� ������ ��� �������� ������ 1.
	long double* arTinyAmounts = new long double[elemCount];

	// �������.
	for (int i = 0; i < elemCount; ++i) 
		arTinyAmounts[i] = arResAmounts[i] = 0;
	

	try {
		// ���������� ��� ����� � ��������� ����������.
		Dot *cur = globalWorld.getDotsArray();						// ������ ����� �������.
		clan::Sizef worldSize = globalWorld.get_worldSize();
		Dot *last = cur + int(worldSize.width * worldSize.height);	// ����� ����� ��������� ����� �������.
		while (cur < last) {

			// � ������ ����� ���������� ��� �������.
			for (int i = 0; i < elemCount; ++i) {
			
				double amnt = cur->getElemAmount(i);

				// ��������� �������� ���������� ��������, �������������� �����������.
				if (amnt < 1.0)
					arTinyAmounts[i] += amnt;
				else
					arResAmounts[i] += amnt;
			}

			// ���� � ����� ���� ��������, ��������� ������� � ��.
			demi::Organism* organism = cur->organism;
			if (organism) {

				std::vector<float>::iterator itAmounts = organism->leftReagentAmounts.begin();
				const demi::ChemReaction &reaction = *organism->get_species()->reaction;
				for (auto &reagent : reaction.leftReagents) {

					// ������� ��������� �������� �� "���" ���������.
					float amnt = *itAmounts++;

					// �������������� ����� � ����� ����������.
					if (amnt < 1.0)
						arTinyAmounts[reagent.elementIndex] += amnt;
					else
						arResAmounts[reagent.elementIndex] += amnt;
				}

			}

			++cur;
		}

		// ������� ����� � ��������.
		for (int i = 0; i < elemCount; ++i)
			arResAmounts[i] += arTinyAmounts[i];

		delete[] arTinyAmounts;
	
	} catch (...) {
		// � ������ ���������� ��������� ������ � ���������.
		delete[] arTinyAmounts;
		throw;
	}
}
