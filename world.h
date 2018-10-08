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
#include "organism.h"
#include "settings_storage.h"
#include "reactions.h"
#include "local_coord.h"
#include "amounts.h"
#include "demi_time.h"


// Глобальная переменная - мир.
class World;
extern World globalWorld;


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
	Dot *getDotsArray() { return arDots; }
	const demi::DemiTime& getModelTime() { return timeBackup; }

	// Доступ к свойствам.
	clan::Size get_worldSize() { return worldSize; }
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
	std::shared_ptr<demi::Species> getSpecies() { return species; }
	const std::shared_ptr<demi::ChemReaction> getReaction(size_t index) { return reactions.at(index); }

	std::mt19937 &getRandomGenerator() { return generator; }

	// Инициализация настроек.
	void setSettingsStorage(SettingsStorage* pSettingsStorage) { pSettings = pSettingsStorage; }
	SettingsStorage*getSettingsStorage() { return pSettings; }

	// Возвращает координаты точки по указанному индексу.
	clan::Point getDotXYFromIndex(size_t index);

	// Возвращает индекс точки для указанных координат.
	size_t getDotIndexFromXY(size_t x, size_t y) { return x + y * worldSize.width; }

	// Объект для подсчёта количества элементов. Испортить состояние объекта невозможно, поэтому выносим его в паблик для удобства.
	Amounts amounts;

private:

	// Текущий момент времени в расчётной модели и в сохранённой для отрисовки на экране копии.
	demi::DemiTime timeModel;
	demi::DemiTime timeBackup;

	// Источник солнечной энергии.
	Solar solar;

	// Количество геотермальных источников.
	size_t energyCount = 0;

	// Геотермальные источники - массив.
	Geothermal *arEnergy = nullptr;

	// Размеры мира.
	clan::Size worldSize;

	// Поверхность земли и копия для отображения на экране.
	Dot *arDots;

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
	SettingsStorage* pSettings;

	// Известные виды организмов. Древовидный список, в вершине - вид протоорганизма.
	std::shared_ptr<demi::Species> species;

	// Химические реакции, доступные для организмов.
	std::vector<std::shared_ptr<demi::ChemReaction>> reactions;

	// Сами организмы.
	std::vector<demi::Organism*> animals;

	std::random_device random_device; // Источник энтропии.
	std::mt19937 generator; // Генератор случайных чисел.
	
	// Для генерации случайного числа. Направление движения при диффузии
	std::uniform_int_distribution<> rnd_angle;

	// Для генерации случайного числа. Приращение координаты при диффузии.
	std::uniform_int_distribution<> rnd_Coord;

	// Позиция протоорганизма для восстановления его каждый ход.
	clan::Point LUCAPos;

	// Обновить состояние.
	void makeTick();

	// Диффузия ресурсов.
	void diffusion();

	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	void fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect);

	// Задаёт местоположение источников геотермальной энергии.
	void addGeothermal(size_t i, const clan::Point &coord);

	// Рабочий поток
	std::thread thread;

	// Флаги для управления потоком.
	bool threadExitFlag = false;		// Если истина, поток должен завершиться.
	bool threadRunFlag = false;		// Если истина, то поток работает и изменяет модель, иначе простаивает.
	bool threadCrashedFlag = false;	// Если истина, значит поток завершился аварийно.
	std::mutex threadMutex;
	std::condition_variable threadEvent;

	// Рабочая функция потока, вычисляющего модель.
	void workerThread();

	// Рекурсивная функция для считывания видов организмов. Закомментируем на далёкое будущее.
	//std::shared_ptr<demi::Species> doReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor);
	// Рекурсивная функция для записи видов организмов, параллельно создаёт словарь названий видов.
	//void doWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies, std::set<std::string> &dict);

	// Вынесено из SaveModel() для удобства. Запись одного организма.
	void doWriteOrganism(clan::File &binFile, std::set<std::string> &dict, demi::Organism* organism);
	demi::Organism* doReadOrganism(clan::File &binFile, std::set<std::string> &dict, const clan::Point &center);

	// Инициализирует массим максимумов на основе имеющихся количеств в точках, используется после загрузки.
	void InitResMaxArray();
};




