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
	// �������� ������ ��� ���������� �������� ���� ��������� � ������������� �������.
	int elemCount = globalWorld.getElemCount() + 2;
	res = new float[elemCount];
	memset(res, 0, sizeof(float) * elemCount);
	isCopy = false;
}

Dot::Dot(const Dot &obj)
{
	// �������� ���� ������������ ����������� - ������� ����� �������, ����� �� ������ ��� ��� ��������� � ������ ������.
	res = obj.res;
	cells = obj.cells;
	organism = obj.organism;
	isCopy = true;
}

Dot& Dot::operator=(const Dot &obj)
{
	// �������� ���� ������������ ����������� - ������� ����� �������, ����� �� ������ ��� ��� ��������� � ������ ������.
	res = obj.res;
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
	return (globalWorld.getElemCount() + 2) * sizeof(float);
}

void Dot::get_color(clan::Colorf &aValue) const
{
	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	//

	// ��������� ���� � ������� ��� �����-�����.
	aValue.set_alpha(clan::max<float, float>(getSolarEnergy(), getGeothermalEnergy()));

	// ������� ���� ����� ����� ����������.
	//
	for (auto &cell : cells) {
		demi::Organism *cellOrganism = cell->organism;
		if (cellOrganism != nullptr) {
			// ��������, �������� �� ����������� ��� ������� ����.
			auto spc = cellOrganism->get_species();
			if (spc->get_visible()) {
				aValue = cellOrganism->isAlive() ? spc->aliveColor : spc->deadColor;

				return;
			}
		}
	}

	// ������� ������ �������� ����� (����� ������� ������).
	aValue.set_red(0.0f);
	aValue.set_green(0.0f);
	aValue.set_blue(0.0f);


	// ������� ���� ���������.
	// ���� ���������� ������ ����� ������ ���������, ��������� ����� ����������� ������. 
	// ������� ����� ���� ���������� ���������������� ���� ��� ���� �����, �� ���� ����� �����. �����
	// � ������� �������� � �������� ��������� ��������.
	//

	// ������� �������.
	float resBright = 0.0f;

	// ���������� � ����� ��� � ���� �������� �����, �� ���� � ���������� ������������� �������������.
	for (int i = 0; i < globalWorld.elemCount; i++) {

		// ���� ������ ����� ���������, �������� ��� �����������.
		if (globalWorld.arResVisible[i]) {

			// ������� (������������) �������� ��������.
			const float curResBright = res[i + 2] / globalWorld.getResMaxValue(i);

			// ���� ������� ����, ���������� ����.
			if (resBright < curResBright) {
				resBright = curResBright;

				// ���������� �������� �������� 4 ����������, ������� ������������ ������ �������.
				clan::Colorf &col = globalWorld.arResColors[i];
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
	return res[index + 2] * 100 / globalWorld.getResMaxValue(index);
}


// =============================================================================
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
// =============================================================================
LocalCoord::LocalCoord(const clan::Pointf &coord) : center(coord),
worldWidth(globalWorld.get_worldSize().width),
worldHeight(globalWorld.get_worldSize().height)
{
};

Dot& LocalCoord::get_dot(float x, float y) const
{
	// �� ����������� ���������� ��������� � ������ ���� �� ������.
	x = roundf(x + center.x);
	if (x < 0)
		x += worldWidth;
	else if (x >= worldWidth)
		x -= worldWidth;

	// �� ��������� ���� ������ ��������.
	y = roundf(y + center.y);
	if (y < 0)
		y = 0;
	else if (y >= worldHeight)
		y = worldHeight - 1;

	//_ASSERT(x < globalWorld.get_worldWidth());
	//_ASSERT(y < globalWorld.get_worldHeight());

	return globalWorld.getDotsArray()[int(x + y * worldWidth)];
}

