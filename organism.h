/* ===============================================================================
	Моделирование эволюции живого мира.
	Модуль для живого организма.
	10 april 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once



namespace demi {

	// Типы клеток - мозг, рецептор, мышца, жир, живот, рот, броня.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

//
// Клетка, базовый тип.
//
class Organism;
class GenericCell
{
public:
	// Местоположение клетки в системе координат организма.
	int x, y;

	//virtual ~BaseCell(void);

	// Возвращает тип клетки.
	virtual CellTypes getCellType() abstract;

	// Надо хранить ссылку на организм.
	Organism *organism = nullptr;
};


//
// Клетка - живот.
class CellAbdomen : public GenericCell
{
	// Возвращает тип клетки.
	virtual CellTypes getCellType() { return cellAbdomen; }
};


//
// Вид организма
//
class Species
{
public:
	// Родительский организм.
	std::weak_ptr<Species> ancestor;

	// Дочерние организмы. Вообще самому организму нет нужды до его производных, но сохранение древовидной структуры
	// необходимо при записи данных на диск, поэтому информация о детях, во-избежание громоздкости, хранится прямо тут.
	// Хотя возможно стоит создать параллельную структуру для хранения информации в дереве - текущий Species, его дети,
	// а отсюда детей убрать.
	std::vector<std::shared_ptr<Species>> descendants;

	// Клетки организма.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// Название организма.
	std::string name;

	// Автор
	std::string author;

	// Видимость вида.
	bool visible;

	void set_visible(bool AVisible) { visible = AVisible; };
	bool get_visible() { return visible; }

	// Программа организма должна храниться в клетках. Вопрос - насколько она целостная? Одна клетка программу сделать не может,
	// есть смысл только в совокупности клеток. Сколько точек запуска программы?
};

//
// Вид-заглушка под неживую природу.
//
//class Inanimal_Species_Dumb : public Species
//{
//public:
//	// Индекс элемента в массиве неживой природы.
//	int inanimal_index;
//
//	Inanimal_Species_Dumb(int elementIndex) {inanimal_index = elementIndex;}
//	void set_visible(bool visible) override;
//	bool get_visible() override;
//};


//
// Организм.
//
class Organism
{
	// Организм имеет списки клеток разных типов.

public:
	Organism(std::shared_ptr<Species> species);
	virtual ~Organism();

	// Тип организма.
	std::shared_ptr<Species> ourSpecies;

	// Местоположение организма в мире (первой клетки живота) и ориентация (0 - север, 1 - северо-восток, 2 - восток и т.д. до 7 - северо-запад).
	clan::Pointf center;
	int angle;

	// Клетки организма.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
	void makeTick();

	// Формирование реакции на диффузию со стороны внешней среды.

	// Формирование реакции на атаку от другого организма.
};

};