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
	const size_t elemCount = globalWorld.getElemCount();

	// ������� ������ ��� ��������.
	delete[] arResAmounts;
	arResAmounts = new unsigned long long[elemCount];

	// ������� ������ ��������.
	memset(arResAmounts, 0, sizeof(unsigned long long) * elemCount);

	// ���������� ��� ����� � ��������� ����������.
	Dot *cur = globalWorld.getDotsArray();						// ������ ����� �������.
	const clan::Size& worldSize = globalWorld.getWorldSize();
	Dot *last = cur + worldSize.width * worldSize.height;		// ����� ����� ��������� ����� �������.
	while (cur < last) {

		// ��������� ������� ������ �����.
		for (size_t i = 0; i != elemCount; ++i)
			arResAmounts[i] += cur->getElemAmount(i);

		// ���� � ����� ���� ��������, ��������� ������� � ��.
		demi::Organism* organism = cur->organism;
		if (organism) {
			demi::organismAmounts_t::const_iterator itAmounts = organism->getLeftReagentAmounts().begin();
			const demi::ChemReaction &reaction = *organism->getChemReaction().get();
			for (auto &reagent : reaction.leftReagents) {
				// ������� ��������� �������� �� "���" ���������.
				demi::organismAmount_t amnt = *itAmounts++;
				if (amnt != 0)
					arResAmounts[reagent.elementIndex] += amnt;
			}
		}

		++cur;
	}
	
}
