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
#include "demi_time.h"


namespace demi {

	// Типы клеток - мозг, рецептор, мышца, жир, живот, рот, броня.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

	// Тип для хранения текущих накопленных веществ в организме.
	typedef uint8_t organismAmount_t;
	typedef std::vector<organismAmount_t> organismAmounts_t;

	const uint16_t cMaxVitality = UINT16_MAX - UINT8_MAX;
//
// Клетка, базовый тип.
//
	class Organism;
class GenericCell
{
public:
	// Местоположение клетки в системе координат организма.
	int x, y;

	GenericCell() : x(0), y(0), organism(nullptr) {}
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
	Species(const std::weak_ptr<Species>& Aancestor,
		const std::string& Aname,
		const std::string& Aauthor,
		bool Avisible,
		uint16_t AfissionBarrier,
		const clan::Color& AaliveColor,
		const clan::Color& AdeadColor,
		const std::shared_ptr<ChemReaction>& Areaction
		);

	// Доступ к полям.
	const std::weak_ptr<Species>& getAncestor() { return ancestor; }
	const std::string& getName() { return name; }
	const std::string& getAuthor() { return author; }
	void setVisible(bool AVisible) { visible = AVisible; };
	bool getVisible() { return visible; }
	const clan::Color& getAliveColor() { return aliveColor; }
	const clan::Color& getDeadColor() { return deadColor; }
	uint16_t getFissionBarrier() { return fissionBarrier; }
	const std::shared_ptr<ChemReaction>& getReaction() { return reaction; }

	// Возвращаем неконстантные ссылки для возможной правки для оптимизации быстродействия.
	std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

	// Возвращает полное название вида в формате автор/вид\автор/вид... Корневой общий для всех вид не включается.
	std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
	//std::string getFullName();

	// Возвращает вид по указанному полному названию. Должна вызываться для корневого вида.
	std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);

	// Для ведения простейшей статистики по количеству живых.
	void incAliveCount() { ++aliveCount; }
	void decAliveCount() { --aliveCount; }
	size_t getAliveCount() const { return aliveCount; }

private:
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
	clan::Color aliveColor, deadColor;

	// Метаболитическая реакция организма.
	std::shared_ptr<ChemReaction> reaction;

	// Начальный порог размножения (будет меняться из-за изменчивости).
	uint16_t fissionBarrier;

	// Количество живых организмов данного вида (для статистики).
	size_t aliveCount = 0;
};


//
// Организм.
//
class Organism
{
public:
	Organism(const clan::Point &Acenter, uint8_t Aangle, uint16_t AfissionBarrier, int32_t Avitality, const DemiTime& Abirthday, uint64_t AancestorsCount, const std::shared_ptr<Species>& species);
	~Organism();

	// Доступ к полям.
	uint8_t getAngle() const { return angle; }
	const LocalCoord& getCenter() const { return center; }
	const std::vector<std::shared_ptr<GenericCell>>& getCells() const { return cells; }
	const DemiTime& getBirthday() const { return birthday; }
	uint64_t getAncestorsCount() const { return ancestorsCount; }
	const std::shared_ptr<Species>& getSpecies() const { return ourSpecies; }
	 organismAmounts_t& getLeftReagentAmounts()  { return leftReagentAmounts; }
	int32_t getVitality() const { return vitality; }
	uint16_t getFissionBarrier() const { return fissionBarrier; }


	// Минимальная энергия метаболизма для активной клетки и для пассивной.
	static uint8_t minActiveMetabolicRate, minInactiveMetabolicRate;

	// Жизненная энергия, ниже которой организм разрушается.
	static int32_t desintegrationVitalityBarrier;

	// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
	// Может вернуть указатель на новый родившийся организм.
	Organism* makeTickAndGetNewBorn();

	// Формирование реакции на диффузию со стороны внешней среды, возвращает истину, если можно переместиться.
	bool canMove() { return true; }

	// Формирование реакции на атаку от другого организма.

	// Безусловное перемещение организма в другую точку.
	void moveTo(const clan::Point &newCenter);

	// Истина, если организм жив. Если нет, то его надо исключить из списка живых.
	bool isAlive() { return vitality > 0; }

	// Истина, если организм уже разложился и его надо уничтожить.
	bool needDesintegration() { return vitality < desintegrationVitalityBarrier; }

	// Вычитает жизненную энергию на неактивное состояние и обновляет статистику.
	void processInactiveVitality();

	// Считывают и сохраняют себя в файл.
	static Organism* createFromFile(clan::File& binFile, const clan::Point& Acenter, const std::shared_ptr<Species>& Aspecies);
	void saveToFile(clan::File& binFile);

private:
	// Местоположение организма в мире (первой клетки живота) и ориентация (0 - север, 1 - северо-восток, 2 - восток и т.д. до 7 - северо-запад).
	LocalCoord center;
	uint8_t angle;

	// Порог размножения для организма (изначально совпадает со значением для вида, потом меняется из-за изменчивости).
	uint16_t fissionBarrier;

	// Текущая накопленная энергия.
	int32_t vitality;

	// Дата и время рождения.
	DemiTime birthday;

	// Количество предков данного вида.
	uint64_t ancestorsCount;

	// Вид организма.
	std::shared_ptr<Species> ourSpecies;

	// Клетки организма.
	std::vector<std::shared_ptr<GenericCell>> cells;

	// Текущие ячейки для хранения вещества перед реакцией.
	organismAmounts_t leftReagentAmounts;

	// Возвращает свободную клетку из окрестностей, если такая есть и истину, иначе ложь.
	bool findFreePlace(clan::Point &point);

	// Возвращает точку, лежащую относительно исходной в указанном направлении с учётом собственного направления.
	void getPointAtDirection(uint8_t direction, clan::Point & dest);
};

};