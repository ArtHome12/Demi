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
#include "world.h"

// =============================================================================
// ����� �� ������ ����������� � ������������ � ������� �������� � ��������� �� ����������.
// =============================================================================
Dot::Dot()
{
	// �������� ������ ��� ��������.
	int elemCount = globalWorld.getElemCount();
	res = new unsigned long long[elemCount];
	memset(res, 0, sizeof(unsigned long long) * elemCount);
	isCopy = false;
}

Dot::Dot(const Dot &obj)
{
	// �������� ���� ������������ ����������� - ������� ����� �������, ����� �� ������ ��� ��� ��������� � ������ ������.
	res = obj.res;
	solarEnergy = obj.solarEnergy;
	geothermalEnergy = obj.geothermalEnergy;
	cells = obj.cells;
	organism = obj.organism;
	isCopy = true;
}

Dot& Dot::operator=(const Dot &obj)
{
	// �������� ���� ������������ ����������� - ������� ����� �������, ����� �� ������ ��� ��� ��������� � ������ ������.
	res = obj.res;
	solarEnergy = obj.solarEnergy;
	geothermalEnergy = obj.geothermalEnergy;
	cells = obj.cells;
	organism = obj.organism;
	isCopy = true;
	return *this;
}



Dot::~Dot()
{
	if (!isCopy)
		delete res;
}

int Dot::getSizeInMemory()
{
	// ����� ������ � ������ ��� ������ - ������ ���������� �������� ���� ��������� � ������������� �������.
	return globalWorld.getElemCount() * sizeof(unsigned long long) + sizeof(float) * 2;
}

void Dot::get_color(clan::Color &aValue) const
{
	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	//

	// ��������� ���� � ������� ��� �����-�����.
	unsigned char alpha = (unsigned char)(clan::max<float, float>(getSolarEnergy(), getGeothermalEnergy()) * 255.0);

	// ������� ���� ����� ����� ����������. ��� ��� ���� �����������, ��� ��������� ����� ������� �� ���������, ���������� ��������� ������ �������.
	//
	try {
		for (auto &cell : cells) {
			demi::Organism *cellOrganism = cell->organism;
			if (cellOrganism != nullptr) {
				// ��������, �������� �� ����������� ��� ������� ����.
				auto spc = cellOrganism->get_species();
				if (spc->get_visible()) {
					aValue = cellOrganism->isAlive() ? spc->aliveColor : spc->deadColor;
					aValue.set_alpha(alpha);
					return;
				}
			}
		}
	}
	catch (...)
	{
		// ������ ����� ���������� ������ ��� ������� �� ������, ���������� ��.
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
	for (int i = 0; i < globalWorld.getElemCount(); i++) {

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
float Dot::getElemAmountPercent(int index) const
{
	return getElemAmount(index) * 100.0f / globalWorld.getResMaxValue(index);
}


// =============================================================================
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
// =============================================================================
LocalCoord::LocalCoord(const clan::Point &coord) : center(coord),
worldWidth(globalWorld.get_worldSize().width),
worldHeight(globalWorld.get_worldSize().height)
{
};

Dot& LocalCoord::get_dot(int x, int y) const
{
	// �� ����������� ���������� ��������� � ������ ���� �� ������.
	x = x + center.x;
	if (x < 0)
		x += worldWidth;
	else if (x >= worldWidth)
		x -= worldWidth;

	// �� ��������� ���� ������ ��������.
	y = y + center.y;
	if (y < 0)
		y = 0;
	else if (y >= worldHeight)
		y = worldHeight - 1;

	//_ASSERT(x < globalWorld.get_worldWidth());
	//_ASSERT(y < globalWorld.get_worldHeight());

	return globalWorld.getDotsArray()[x + y * worldWidth];
}

