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

#include "local_coord.h"
#include "demi_time.h"
#include "gene.h"

namespace demi {

	// Типы клеток - мозг, рецептор, мышца, жир, живот, рот, броня.
	enum CellTypes {cellBrain, cellReceptor, cellMuscle, cellAdipose, cellAbdomen, cellMouth, cellArmor};

	// Некоторые предварительные объявления.
	class Organism;
	class DemiTime;
	class ChemReaction;


//
// Клетка, базовый тип.
//
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
// Организм.
//
class Organism
{
public:
	Organism(const clan::Point &Acenter, uint8_t Aangle, int32_t Avitality, const DemiTime& Abirthday, uint64_t AancestorsCount, const std::shared_ptr<Species>& species);
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
	const std::shared_ptr<ChemReaction>& getChemReaction() const { return ourReaction; }

	// Минимальная энергия метаболизма для активной клетки и для пассивной.
	static uint8_t minActiveMetabolicRate, minInactiveMetabolicRate;

	// Жизненная энергия, ниже которой организм разрушается.
	static int32_t desintegrationVitalityBarrier;

	// Процессорное время организма для формирования поведения - поедания пищи, атаки, разворота, перемещения, размножения.
	// Возвращает истину, если готов делиться.
	bool makeTick();

	// Ополовинивает жизненную энергию (используется после деления).
	int32_t halveVitality() { return vitality /= 2; }

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
	static Organism* createFromFile(clan::IODevice& binFile, const clan::Point& Acenter, const std::shared_ptr<Species>& Aspecies);
	void saveToFile(clan::IODevice& binFile);

	// Возвращает имя генотипа организма.
	std::string getGenotypeName() { return ourSpecies->getGenotypeName(); }

	// Возвращает имя вида организма.
	std::string getSpeciesName() { return ourSpecies->getSpeciesName(); }

	// Возвращает свободную клетку из окрестностей, если такая есть и истину, иначе ложь.
	bool findFreePlace(clan::Point &point);

	// Должны вызываться перед доступом к организму.
	// Делает одну попытку установить блокировку и возвращает истину, если удалось. Если ложь, обращаться к полям организма небезопасно.
	bool tryLock() { return !lockFlag.test_and_set(std::memory_order_acquire); }
	// Останавливает выполнение до тех пор, пока не удастся установить блокировку.
	void lock() { for (size_t i = 0; !tryLock(); ++i) if (i % 100 == 0) std::this_thread::yield(); }
	// Снимает блокировку.
	void unlock() { lockFlag.clear(std::memory_order_release); }


private:
	// Местоположение организма в мире (первой клетки живота) и ориентация (0 - север, 1 - северо-восток, 2 - восток и т.д. до 7 - северо-запад).
	LocalCoord center;
	uint8_t angle;

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

	// Указатель на реакцию, извлеченный из гена, для ускорения.
	std::shared_ptr<ChemReaction> ourReaction;

	// Текущие ячейки для хранения вещества перед реакцией.
	organismAmounts_t leftReagentAmounts;

	// Порог размножения, извлечённый из гена для ускорения, либо 2 как минимум.
	int32_t breedingReserve = 2;

	// Для блокировки одновременного доступа из разных потоков (расчётного и интерфейсного).
	std::atomic_flag lockFlag;

	// Возвращает точку, лежащую относительно исходной в указанном направлении с учётом собственного направления.
	void getPointAtDirection(uint8_t direction, clan::Point & dest);

	// Уменьшает количество живых организмов.
	void decAliveCount();
};

};
