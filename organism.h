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

#include "reactions.h"
#include "local_coord.h"



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

	GenericCell() {}
	GenericCell(int Ax, int Ay, Organism * Aorganism) : x(Ax), y(Ay), organism(Aorganism) {}


	//virtual ~GenericCell(void);

	// Возвращает тип клетки.
	virtual CellTypes getCellType() abstract;

	// Возвращает копию клетки.
	virtual GenericCell *getClone() abstract;

	// Надо хранить ссылку на организм.
	Organism *organism = nullptr;
};


//
// Клетка - живот.
class CellAbdomen : public GenericCell
{
	// Возвращает тип клетки.
	virtual CellTypes getCellType() override { return cellAbdomen; }

	// Возвращает копию клетки.
	virtual GenericCell *getClone() override { return new CellAbdomen(*this); }

	// Делаем реакцию 
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

	// Цвет для живого и мёртвого организмов.
	clan::Colorf aliveColor, deadColor;

	// Метаболитическая реакция организма.
	std::shared_ptr<ChemReaction> reaction;

	// Начальный порог размножения (будет меняться из-за изменчивости).
	float fissionBarrier;

	void set_visible(bool AVisible) { visible = AVisible; };
	bool get_visible() { return visible; }

	// Возвращает полное название вида в формате автор/вид\автор/вид... Корневой общий для всех вид не включается.
	std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
	std::string getFullName();

	// Возвращает вид по указанному полному названию. Должна вызываться для корневого вида.
	Species *getSpeciesByFullName(std::string fullName);
};


//
// Организм.
//
class Organism
{
public:
	Organism(std::shared_ptr<Species> species, const clan::Pointf &Acenter, int Aangle, float Avitality, float AfissionBarrier);
	~Organism();

	// Местоположение организма в мире (первой клетки живота) и ориентация (0 - север, 1 - северо-восток, 2 - восток и т.д. до 7 - северо-запад).
	int angle;
	LocalCoord center;

	// Клетки организма.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// Доступ к полям.
	std::shared_ptr<Species> get_species() { return ourSpecies; }

	// Минимальная энергия метаболизма для активной клетки и для пассивной.
	static float minActiveMetabolicRate, minInactiveMetabolicRate;

	// Жизненная энергия, ниже которой организм разрушается.
	static float desintegrationVitalityBarrier;

	// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
	// Может вернуть указатель на новый родившийся организм.
	Organism* makeTickAndGetNewBorn();

	// Формирование реакции на диффузию со стороны внешней среды, возвращает истину, если можно переместиться.
	bool canMove() { return true; }

	// Формирование реакции на атаку от другого организма.

	// Безусловное перемещение организма в другую точку.
	void moveTo(const clan::Pointf &newCenter);

	// Истина, если организм жив. Если нет, то его надо исключить из списка живых.
	bool isAlive() { return vitality > 0; }

	// Истина, если организм уже разложился и его надо уничтожить.
	bool needDesintegration() { return vitality < desintegrationVitalityBarrier; }

	float getVitality() { return vitality; }
	float getFissionBarrier() { return fissionBarrier; }

	// Вычитает жизненную энергию на неактивное состояние.
	void processInactiveVitality() { vitality -= minActiveMetabolicRate; }

private:
	// Вид организма.
	std::shared_ptr<Species> ourSpecies;

public:
	// Текущие ячейки для хранения вещества перед реакцией.
	std::vector<float> leftReagentAmounts;

private:
	// Текущая накопленная энергия.
	float vitality;

	// Порог размножения для организма (изначально совпадает со значением для вида, потом меняется из-за изменчивости).
	float fissionBarrier;

	// Возвращает свободную клетку из окрестностей, если такая есть и истину, иначе ложь.
	bool findFreePlace(clan::Pointf &point);

	// Возвращает точку, лежащую относительно исходной в указанном направлении с учётом собственного направления.
	void getPointAtDirection(int direction, clan::Pointf & dest);
};

};