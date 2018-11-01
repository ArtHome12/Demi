/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для относительной системы координат.
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
// Точка на земной поверхности с координатами и списком ресурсов с указанием их количества.
// =============================================================================
Dot::Dot()
{
	// Выделяем память под элементы.
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
	// Возвращает цвет для точки на основе имеющихся ресурсов, передача по ссылке для оптимизации.
	//

	// Солнечный свет и энергия это альфа-канал.
	unsigned char alpha = (unsigned char)(clan::max<float, float>(getSolarEnergy(), getGeothermalEnergy()) * 255.0f);

	// Сначала ищем среди живых организмов.
	//
	/*for (auto &cell : cells) { код с поддержкой многоклеточных организмов пока неактуален. На будущее!!!
		demi::Organism *cellOrganism = cell->organism;
		if (cellOrganism != nullptr) {
			// Проверим, включено ли отображение для данного вида.
			auto spc = cellOrganism->getSpecies();
			if (spc->getVisible()) {
				aValue = cellOrganism->isAlive() ? spc->getAliveColor() : spc->getDeadColor();
				aValue.set_alpha(alpha);
				return;
			}
		}
	}*/
	if (organism && organism->tryLock()) {
		// Проверим, включено ли отображение для данного вида.
		auto spc = organism->getSpecies();
		if (spc->getVisible()) {
			aValue = organism->isAlive() ? spc->getAliveColor() : spc->getDeadColor();
			aValue.set_alpha(alpha);
			organism->unlock();
			return;
		}
		organism->unlock();
	}

	// Папали сюда, значит определяем на основе неживых элементов.
	// Очистим старое значение цвета (самый быстрый способ).
	aValue.set_red(0);
	aValue.set_green(0);
	aValue.set_blue(0);
	aValue.set_alpha(alpha);

	// Выводим цвет минералов.
	// Если складываем вместе цвета разных элементов, возникают точки неожиданных цветов. 
	// Выходом может быть отображать преимущественный цвет для всей точки, то есть самый яркий. Начнём
	// с первого элемента и проверим остальные элементы.
	//

	// Текущая яркость.
	float resBright = 0.0f;

	// Перебираем в цикле все и ищем наиболее яркий, то есть с наибольшей относительной концентрацией.
	for (size_t i = 0; i != globalWorld.getElemCount(); i++) {

		// Ищем только среди элементов, выранных для отображения.
		if (globalWorld.getResVisibility(i)) {

			// Яркость (концентрация) текущего элемента.
			const float curResBright = getElemAmountPercent(i);

			// Если яркость выше, запоминаем цвет.
			if (resBright < curResBright) {
				resBright = curResBright;

				// Встроенный оператор изменяет 4 компоненты, поэтому присваивание делаем вручную.
				const clan::Color &col = globalWorld.getResColors(i);
				aValue.x = col.x;
				aValue.y = col.y;
				aValue.z = col.z;
			}
		}
	}
}

// Количество указанного элемента в процентах.
float Dot::getElemAmountPercent(size_t index) const
{
	return getElemAmount(index) * 100.0f / globalWorld.getResMaxValue(index);
}


// =============================================================================
// Локальные координаты - центр всегда в точке 0, 0 и можно адресовать отрицательные координаты.
// =============================================================================
LocalCoord::LocalCoord(const clan::Point &coord) : center(coord),
worldWidth(globalWorld.getWorldSize().width),
worldHeight(globalWorld.getWorldSize().height)
{
};

Dot& LocalCoord::get_dot(int x, int y) const
{
	// По горизонтали координату переносим с одного края на другой.
	x = x + center.x;
	if (x < 0)
		x += int(worldWidth);
	else if (x >= int(worldWidth))
		x -= int(worldWidth);

	// По вертикали.
	y = y + center.y;
	if (y < 0)
		y += int(worldHeight);
	else if (y >= int(worldHeight))
		y = int(worldHeight - 1);

	//_ASSERT(x < globalWorld.get_worldWidth());
	//_ASSERT(y < globalWorld.get_worldHeight());

	return globalWorld.getDotsArray()[size_t(x) + size_t(y) * worldWidth];
}

