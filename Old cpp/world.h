/* ===============================================================================
	Моделирование эволюции живого мира.
	Модуль для основных компонентов - точка, пространство.
	24 august 2013.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

#include <set>
#include "amounts.h"
#include "demi_time.h"


// Глобальная переменная - мир.
class World;
extern World globalWorld;

class SettingsStorage;

namespace demi {
	class Dot;
	class ChemReaction;
	class GenotypesTree;
	class Organism;
}

//
// Источник солнечной энергии
//
class Solar {
private:

	// Возвращает позицию для солнца в зависимости от времени.
	clan::Point getPos(const demi::DemiTime &timeModel);

public:

	// Облучает энергией для указанного момента времени землю.
	void Shine(const demi::DemiTime &timeModel);
};


//
// Источник геотермальной энергии
//
class Geothermal {
public:
	clan::Point coord;
};

//
// Земная поверхность, двумерный массив точек.
//
class World {

//	friend Dot;
//	friend Solar;

public:
	World();
	~World();

	// Считывает модель из xml-файла и сохраняет.
	void loadModel(const std::string &filename);
	void saveModel(const std::string &filename);

	// Приостанавливает или продолжает расчёт модели
	void runEvolution(bool is_active);

	// Начинает расчёт заново.
	void resetModel(const std::string &modelFilename, const std::string &defaultFilename);

	// Возвращает точки поверхности.
	demi::Dot *getDotsArray() { return arDots; }
	const demi::DemiTime& getModelTime() { return timeModel; }

	// Доступ к свойствам.
	clan::Size getWorldSize() { return worldSize; }
	size_t getElemCount() { return elemCount; }
	size_t getEnergyCount() { return energyCount; }
	size_t getLightRadius() { return lightRadius; }
	size_t getTropicHeight() { return tropicHeight; }
	unsigned long long getResMaxValue(size_t index) { return arResMax[index]; }
	const clan::Color &getResColors(size_t index) { return arResColors[index]; }
	const std::string &getResName(size_t index) { return arResNames[index]; }
	const bool getResVisibility(size_t index) { return arResVisible[index]; }
	void setResVisibility(size_t index, bool visible) { arResVisible[index] = visible; }
	const clan::Point &getAppearanceTopLeft() { return appearanceTopLeft; }
	void setAppearanceTopLeft(const clan::Point& newTopLeft) { appearanceTopLeft = newTopLeft; }
	float getAppearanceScale() { return appearanceScale; }
	void setAppearanceScale(float newScale) { appearanceScale = newScale; }
	const std::shared_ptr<demi::ChemReaction> getReaction(size_t index) { return reactions.at(index); }

	std::mt19937 &getRandomGenerator() { return generator; }

	// Инициализация настроек.
	void setSettingsStorage(SettingsStorage* pSettingsStorage) { pSettings = pSettingsStorage; }
	SettingsStorage*getSettingsStorage() { return pSettings; }

	// Возвращает координаты точки по указанному индексу.
	clan::Point getDotXYFromIndex(size_t index);

	// Возвращает индекс точки для указанных координат.
	size_t getDotIndexFromXY(size_t x, size_t y) { return x + y * worldSize.width; }

	// Вычисляет вероятность мутации и возвращает истину, если она должна состояться.
	bool activateMutation();

	// Объект для подсчёта количества элементов. Испортить состояние объекта невозможно, поэтому выносим его в паблик для удобства.
	Amounts amounts;

	// Объект для связи между интерфейсом и генотипами/видами модели. Древовидный список, в вершине - вид протоорганизма.
	std::shared_ptr<demi::GenotypesTree> genotypesTree;

	// Возвращает скорость модели в тиках в секунду.
	int getUpdatesPerSecond() { return current_tps; }

private:
	// Текущий момент времени в расчётной модели.
	demi::DemiTime timeModel;

	// Источник солнечной энергии.
	Solar solar;

	// Количество геотермальных источников.
	size_t energyCount = 0;

	// Геотермальные источники - массив.
	Geothermal *arEnergy = nullptr;

	// Размеры мира.
	clan::Size worldSize;

	// Поверхность земли.
	demi::Dot *arDots = nullptr;

	// Свойства, используемые для отображения модели - координата левого верхнего угла, масштаб.
	clan::Point appearanceTopLeft;
	float appearanceScale = 1.0f;

	// Максимальный радиус освещённости вокруг местонахождения солнца, составляет 90% высоты мира.
	// Эта же величина и максимальная яркость солнца - в самой удалённой точке она была минимальной и равной 1, соответственно в центре - равна радиусу.
	size_t lightRadius = 0;

	// Ход солнца по вертикали, обусловленный наклоном земной оси, примерно sin(23.5)=0.4 или по 10% с каждой стороны от экватора.
	size_t tropicHeight = 0;

	// Цвета элементов.
	clan::Color* arResColors = nullptr;

	// Количество химических элементов, существующих в модели.
	size_t elemCount = 0;

	// Названия ресурсов. Инициализируется извне.
	std::string *arResNames = nullptr;
	
	// Включено отображение элемента или нет (видимость определяется пользователем в настройках).
	bool *arResVisible = nullptr;

	// Массив максимальных значений ресурсов для целей визуализации на экране.
	unsigned long long *arResMax = nullptr;

	// Летучесть ресурса для диффузии, значение от 0 до 1.
	float* arResVolatility = nullptr;

	// Настройки программы - не используем shared_ptr, так как к моменту вызова деструктора ~SettingsStorage графическая 
	// подсистема должна ещё существовать - а объект World глобальный и может умереть позже неё.
	SettingsStorage* pSettings = nullptr;

	// Химические реакции, доступные для организмов.
	std::vector<std::shared_ptr<demi::ChemReaction>> reactions;

	std::random_device random_device; // Источник энтропии.
	std::mt19937 generator; // Генератор случайных чисел.
	
	// Для генерации случайного числа. Направление движения.
	std::uniform_int_distribution<> rnd_angle;

	// Для генерации случайного числа. Приращение координаты.
	std::uniform_int_distribution<> rnd_Coord;

	// Для генерации случайного числа. Вероятность мутации.
	std::uniform_int_distribution<> rnd_Mutation;

	// Позиция протоорганизма для восстановления его каждый ход.
	clan::Point LUCAPos;

	// Переменные для вычисления скорости расчёта модели.
	int num_updates_in_2_seconds = 0;
	uint64_t update_frame_start_time = 0;
	int current_tps = 0;

	// Обновить состояние.
	void makeTick();

	// Перебор всех точек и вычисления.
	void processDots();

	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	void fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect);

	// Задаёт местоположение источников геотермальной энергии.
	void addGeothermal(size_t i, const clan::Point &coord);

	// Флаги для управления потоком.
	bool threadExitFlag = false;	// Если истина, поток должен завершиться.
	bool threadRunFlag = false;		// Если истина, то поток работает и изменяет модель, иначе простаивает.
	bool threadCrashedFlag = false;	// Если истина, значит поток завершился аварийно.
	std::mutex threadMutex;
	std::condition_variable threadEvent;

	// Рабочий поток, объявляется после используемых в нём переменных.
	std::thread thread;

	// Рабочая функция потока, вычисляющего модель.
	void workerThread();

	// Вынесено из SaveModel() для удобства. Запись одного организма.
	void doWriteOrganism(clan::IODevice &binFile, speciesDict_t& speciesDict, demi::Organism* organism);
	demi::Organism* doReadOrganism(clan::IODevice &binFile, speciesDict_t& speciesDict, const clan::Point &center);

	// Инициализирует массим максимумов на основе имеющихся количеств в точках, используется после загрузки, а также при остановке.
	void InitResMaxArray();

	// Для разгрузки функций loadModel, saveModel.
	void doLoadInanimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc);
	void doLoadReactions(std::shared_ptr<clan::XMLResourceDocument>& resDoc);
	void doLoadAnimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc);
	void doLoadGenotypes(clan::DomElement& node, std::shared_ptr<demi::GenotypesTree> ancestorTreeItem, std::map<std::string, std::vector<std::string>>& geneDatas);
	void doLoadBinary(const std::string &filename);
	void doSaveSettings(std::shared_ptr<clan::XMLResourceDocument>& resDoc);
	void doSaveBinary(const std::string &filename);

	// Вычисляет скорость расчёта модели, количество тиков в секунду.
	void calculateTPS();

	// Возвращает индекс элемента по названию. Когда элементов или реакций станет много, надо будет оптимизировать.
	size_t findElemIndex(const std::string& elemName);
};




