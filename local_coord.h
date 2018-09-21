/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для относительной системы координат.
03 january 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

namespace demi
{
	class GenericCell;
	class Organism;
}

//
// Точка на земной поверхности со списком ресурсов с указанием их количества.
//
class Dot {
public:

	// Солнечная энергия в точке.
	float getSolarEnergy() const { return  solarEnergy; }
	void setSolarEnergy(float newVal) { solarEnergy = newVal; }

	// Геотермальная энергия в точке.
	float getGeothermalEnergy() const { return geothermalEnergy; }
	void setGeothermalEnergy(float newVal) { geothermalEnergy = newVal; }

	// Количество указанного элемента в абсолютной величине.
	unsigned long long getElemAmount(int index) const { return res[index]; }
	void setElementAmount(int index, unsigned long long amount) { res[index] = amount; }

	// Количество указанного элемента в процентах.
	float getElemAmountPercent(int index) const;

	// Имеющиеся в ней ресурсы - первые два элемента это солнечная и геотермальная энергии (для ускорения сохранения).
	unsigned long long *res;

	// Конструктор, массив ресов обнуляем.
	Dot();
	Dot(const Dot &obj);
	~Dot();
	Dot& operator=(const Dot &obj);

	// Возвращает цвет для точки на основе имеющихся ресурсов, передача по ссылке для оптимизации.
	void get_color(clan::Color &aValue) const;

	// Клетки организмов, размещённые в точке.
	std::vector<std::shared_ptr<demi::GenericCell>> cells;

	// Указатель на организм в точке или nullptr. У этого организма одна из клеток должна быть в текущей точке. Для оптимизации,
	// чтобы не искать перебором среди клеток.
	demi::Organism* organism = nullptr;

private:
	// Установлено в истину для точек после конструктора копирования. Копии не освобождают память из-под res.
	bool isCopy;

	float solarEnergy = 0.0f;
	float geothermalEnergy = 0.0f;
};


//
// Локальные координаты - центр всегда в точке 0, 0 и можно адресовать отрицательные координаты.
//
class LocalCoord {
public:
	LocalCoord(const clan::Point &coord);

	// Возвращает точку в локальных координаах.
	Dot& get_dot(int x, int y) const;
	Dot& get_dot(const clan::Point &coord) const { return get_dot(coord.x, coord.y); }

	// Возвращает адрес точки в глобальных координатах.
	clan::Point getGlobalPoint() { return center; }
	clan::Point getGlobalPoint(const clan::Point &localPoint) { return center + localPoint; }

private:
	// Массив с точками поверхности.
	Dot *dots;

	// Размеры мира на момент создания, для оптимизаци.
	int worldWidth;
	int worldHeight;

	// Центр локальных координат, выраженный в глобальных координатах.
	clan::Point center;
};


