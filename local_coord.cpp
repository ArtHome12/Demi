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
#include "world.h"

// =============================================================================
// Точка на земной поверхности с координатами и списком ресурсов с указанием их количества.
// =============================================================================
Dot::Dot()
{
	// Выделяем память под элементы.
	int elemCount = globalWorld.getElemCount();
	res = new unsigned long long[elemCount];
	memset(res, 0, sizeof(unsigned long long) * elemCount);
	isCopy = false;
}

Dot::Dot(const Dot &obj)
{
	// Основная цель конструктора копирования - создать копию вектора, чтобы не падать при его изменении в другом потоке.
	res = obj.res;
	solarEnergy = obj.solarEnergy;
	geothermalEnergy = obj.geothermalEnergy;
	cells = obj.cells;
	organism = obj.organism;
	isCopy = true;
}

Dot& Dot::operator=(const Dot &obj)
{
	// Основная цель конструктора копирования - создать копию вектора, чтобы не падать при его изменении в другом потоке.
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
	// Объём памяти в байтах под объект - массив количества ресурсов плюс солнечная и геотермальная энергии.
	return globalWorld.getElemCount() * sizeof(unsigned long long) + sizeof(float) * 2;
}

void Dot::get_color(clan::Color &aValue) const
{
	// Возвращает цвет для точки на основе имеющихся ресурсов, передача по ссылке для оптимизации.
	//

	// Солнечный свет и энергия это альфа-канал.
	unsigned char alpha = (unsigned char)(clan::max<float, float>(getSolarEnergy(), getGeothermalEnergy()) * 255.0);

	// Сначала ищем среди живых организмов. Так как есть вероятность, что расчётный поток изменит их состояние, игнорируем возможные ошибки доступа.
	//
	try {
		for (auto &cell : cells) {
			demi::Organism *cellOrganism = cell->organism;
			if (cellOrganism != nullptr) {
				// Проверим, включено ли отображение для данного вида.
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
		// Ошибки могли возникнуть только при доступе на чтение, игнорируем их.
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
	for (int i = 0; i < globalWorld.getElemCount(); i++) {

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
float Dot::getElemAmountPercent(int index) const
{
	return getElemAmount(index) * 100.0f / globalWorld.getResMaxValue(index);
}


// =============================================================================
// Локальные координаты - центр всегда в точке 0, 0 и можно адресовать отрицательные координаты.
// =============================================================================
LocalCoord::LocalCoord(const clan::Point &coord) : center(coord),
worldWidth(globalWorld.get_worldSize().width),
worldHeight(globalWorld.get_worldSize().height)
{
};

Dot& LocalCoord::get_dot(int x, int y) const
{
	// По горизонтали координату переносим с одного края на другой.
	x = x + center.x;
	if (x < 0)
		x += worldWidth;
	else if (x >= worldWidth)
		x -= worldWidth;

	// По вертикали пока просто отрезаем.
	y = y + center.y;
	if (y < 0)
		y = 0;
	else if (y >= worldHeight)
		y = worldHeight - 1;

	//_ASSERT(x < globalWorld.get_worldWidth());
	//_ASSERT(y < globalWorld.get_worldHeight());

	return globalWorld.getDotsArray()[x + y * worldWidth];
}

