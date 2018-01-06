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
	float getSolarEnergy() const { return res[0]; }
	void setSolarEnergy(float newVal) { res[0] = newVal; }

	// Геотермальная энергия в точке.
	float getGeothermalEnergy() const { return res[1]; }
	void setGeothermalEnergy(float newVal) { res[1] = newVal; }

	// Количество указанного элемента в абсолютной величине.
	float getElemAmount(int index) const { return res[index + 2]; }
	void setElementAmount(int index, float amount) { res[index + 2] = amount; }

	// Количество указанного элемента в процентах.
	float getElemAmountPercent(int index) const;


	// Объём памяти в байтах под объект - массив количества ресурсов плюс солнечная и геотермальная энергии.
	static int getSizeInMemory();

	// Имеющиеся в ней ресурсы - первые два элемента это солнечная и геотермальная энергии (для ускорения сохранения).
	float *res;

	// Конструктор, массив ресов обнуляем.
	Dot();
	~Dot();

	// Возвращает цвет для точки на основе имеющихся ресурсов, передача по ссылке для оптимизации.
	void get_color(clan::Colorf &aValue) const;

	// Клетки организмов, размещённые в точке.
	std::vector<std::shared_ptr<demi::GenericCell>> cells;

	// Указатель на организм в точке или nullptr. У этого организма одна из клеток должна быть в текущей точке. Для оптимизации,
	// чтобы не искать перебором среди клеток.
	demi::Organism* organism = nullptr;
};


//
// Локальные координаты - центр всегда в точке 0, 0 и можно адресовать отрицательные координаты.
//
class LocalCoord {
public:
	LocalCoord(const clan::Pointf &coord);

	Dot& get_dot(float x, float y) const;

private:
	// Массив с точками поверхности.
	Dot *dots;

	// Размеры мира на момент создания, для оптимизаци.
	float worldWidth;
	float worldHeight;

	// Центр локальных координат, выраженный в глобальных координатах.
	clan::Pointf center;
};


