/* ===============================================================================
������������� �������� ������ ����.
������ ��� ������������� ������� ���������.
03 january 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "local_coord.h"
#include "organism.h"
#include "world.h"

using namespace demi;

// =============================================================================
// ����� �� ������ ����������� � ������������ � ������� �������� � ��������� �� ����������.
// =============================================================================
Dot::Dot()
{
	// �������� ������ ��� ��������.
	size_t elemCount = globalWorld.getElemCount();
	res = new uint64_t[elemCount];
	memset(res, 0, sizeof(uint64_t) * elemCount);
}

Dot::~Dot()
{
	delete[] res;
}

void Dot::getColor(clan::Color &aValue) const
{
	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	//

	// ��������� ���� � ������� ��� �����-�����.
	unsigned char alpha = (unsigned char)(clan::max<float, float>(getSolarEnergy(), getGeothermalEnergy()) * 255.0f);

	// ������� ���� ����� ����� ����������.
	//
	/*for (auto &cell : cells) { ��� � ���������� �������������� ���������� ���� ����������. �� �������!!!
		demi::Organism *cellOrganism = cell->organism;
		if (cellOrganism != nullptr) {
			// ��������, �������� �� ����������� ��� ������� ����.
			auto spc = cellOrganism->getSpecies();
			if (spc->getVisible()) {
				aValue = cellOrganism->isAlive() ? spc->getAliveColor() : spc->getDeadColor();
				aValue.set_alpha(alpha);
				return;
			}
		}
	}*/
	if (organism && organism->tryLock()) {
		// ��������, �������� �� ����������� ��� ������� ����.
		auto spc = organism->getSpecies();
		if (spc->getVisible()) {
			aValue = organism->isAlive() ? spc->getAliveColor() : spc->getDeadColor();
			aValue.set_alpha(alpha);
			organism->unlock();
			return;
		}
		organism->unlock();
	}

	// ������ ����, ������ ���������� �� ������ ������� ���������.
	// ������� ������ �������� ����� (����� ������� ������).
	aValue.set_red(0);
	aValue.set_green(0);
	aValue.set_blue(0);
	aValue.set_alpha(alpha);

	// ������� ���� ���������.
	// ���� ���������� ������ ����� ������ ���������, ��������� ����� ����������� ������. 
	// ������� ����� ���� ���������� ���������������� ���� ��� ���� �����, �� ���� ����� �����. �����
	// � ������� �������� � �������� ��������� ��������.
	//

	// ������� �������.
	float resBright = 0.0f;

	// ���������� � ����� ��� � ���� �������� �����, �� ���� � ���������� ������������� �������������.
	for (size_t i = 0; i != globalWorld.getElemCount(); i++) {

		// ���� ������ ����� ���������, �������� ��� �����������.
		if (globalWorld.getResVisibility(i)) {

			// ������� (������������) �������� ��������.
			const float curResBright = getElemAmountPercent(i);

			// ���� ������� ����, ���������� ����.
			if (resBright < curResBright) {
				resBright = curResBright;

				// ���������� �������� �������� 4 ����������, ������� ������������ ������ �������.
				const clan::Color &col = globalWorld.getResColors(i);
				aValue.x = col.x;
				aValue.y = col.y;
				aValue.z = col.z;
			}
		}
	}
}

// ���������� ���������� �������� � ���������.
float Dot::getElemAmountPercent(size_t index) const
{
	return getElemAmount(index) * 100.0f / globalWorld.getResMaxValue(index);
}


// =============================================================================
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
// =============================================================================
LocalCoord::LocalCoord(const clan::Point &coord) : center(coord),
worldWidth(globalWorld.getWorldSize().width),
worldHeight(globalWorld.getWorldSize().height)
{
};

Dot& LocalCoord::get_dot(int x, int y) const
{
	// �� ����������� ���������� ��������� � ������ ���� �� ������.
	x = x + center.x;
	if (x < 0)
		x += int(worldWidth);
	else if (x >= int(worldWidth))
		x -= int(worldWidth);

	// �� ���������.
	y = y + center.y;
	if (y < 0)
		y += int(worldHeight);
	else if (y >= int(worldHeight))
		y = int(worldHeight - 1);

	//_ASSERT(x < globalWorld.get_worldWidth());
	//_ASSERT(y < globalWorld.get_worldHeight());

	return globalWorld.getDotsArray()[size_t(x) + size_t(y) * worldWidth];
}

