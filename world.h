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


// Продолжительность года в днях.
const int cDaysInYear = 365;

// Количество тиков в сутках, приравняем к секундам 60с*60м*24ч=86400
const int cTicksInDay = 86400;
//const int cTicksInDay = 10;

// Глобальная переменная - мир.
class World;
extern World globalWorld;


//
// Формат мирового времени
//
class DemiTime {
public:
	// Год, от 1 до бесконечности.
	unsigned int year = 1;

	// День года, от 1 до cDaysInYear включительно.
	unsigned int day = 1;

	// Момент внутри дня, от 0 до cTicksInDay
	unsigned int sec = 0;

	// Прибавляет один тик.
	void MakeTick();

	// Возвращает строку с временем.
	std::string getDateStr() const;

	const bool operator!=(const DemiTime& rv) const {
		return (year != rv.year) || (day != rv.day) || (sec != rv.sec);
	}

	DemiTime &operator=(const DemiTime& rv) {
		year = rv.year;
		day = rv.day;
		sec = rv.sec;
		return *this;
	}
};

//
// Источник солнечной энергии
//
class Solar {
private:

	// Возвращает позицию для солнца в зависимости от времени.
	clan::Pointf getPos(const DemiTime &timeModel);

public:

	// Облучает энергией для указанного момента времени землю.
	void Shine(const DemiTime &timeModel);
};


//
// Источник геотермальной энергии
//
class Geothermal {
public:
	clan::Pointf coord;
};


//
// Земная поверхность, двумерный массив точек.
//
class World {

	friend Dot;
	friend Solar;

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
	DemiTime getModelTime() { return timeBackup; }

	// Доступ к свойствам.
	clan::Sizef get_worldSize() { return worldSize; }
	int getElemCount() { return elemCount; }
	int getEnergyCount() { return energyCount; }
	float getLightRadius() { return lightRadius; }
	float getTropicHeight() { return tropicHeight; }
	float getResMaxValue(int index) { return arResMax[index]; }
	const std::string &getResName(int index) { return arResNames[index]; }
	const bool getResVisibility(int index) { return arResVisible[index]; }
	void setResVisibility(int index, bool visible) { arResVisible[index] = visible; }
	const clan::Pointf &getAppearanceTopLeft() { return appearanceTopLeft; }
	void setAppearanceTopLeft(const clan::Pointf newTopLeft) { appearanceTopLeft = newTopLeft; }
	float getAppearanceScale() { return appearanceScale; }
	void setAppearanceScale(float newScale) { appearanceScale = newScale; }
	std::shared_ptr<demi::Species> getSpecies() { return species; }

	std::mt19937 &getRandomGenerator() { return generator; }

	// Инициализация настроек.
	void setSettingsStorage(SettingsStorage* pSettingsStorage) { pSettings = pSettingsStorage; }

	// Возвращает координаты точки по указанному индексу.
	clan::Pointf getXYFromIndex(int index);

private:

	// Текущий момент времени в расчётной модели и в сохранённой для отрисовки на экране копии.
	DemiTime timeModel;
	DemiTime timeBackup;

	// Источник солнечной энергии.
	Solar solar;

	// Количество геотермальных источников.
	int energyCount = 0;

	// Геотермальные источники - массив.
	Geothermal *arEnergy = nullptr;

	// Размеры мира.
	clan::Sizef worldSize;

	// Поверхность земли и копия для отображения на экране.
	Dot *arDots;

	// Свойства, используемые для отображения модели - координата левого верхнего угла, масштаб.
	clan::Pointf appearanceTopLeft;
	float appearanceScale = 1.0f;

	// Максимальный радиус освещённости вокруг местонахождения солнца, составляет 90% высоты мира.
	// Эта же величина и максимальная яркость солнца - в самой удалённой точке она была минимальной и равной 1, соответственно в центре - равна радиусу.
	float lightRadius = 0;

	// Ход солнца по вертикали, обусловленный наклоном земной оси, примерно sin(23.5)=0.4 или по 10% с каждой стороны от экватора.
	float tropicHeight = 0;

	// Цвета элементов.
	clan::Colorf* arResColors = nullptr;

	// Количество химических элементов, существующих в модели.
	int elemCount = 0;

	// Названия ресурсов. Инициализируется извне.
	std::string *arResNames = nullptr;
	
	// Включено отображение элемента или нет (видимость определяется пользователем в настройках).
	bool *arResVisible = nullptr;

	// Массив максимальных значений ресурсов для целей визуализации на экране.
	float *arResMax = nullptr;

	// Летучесть ресурса для диффузии, значение от 0 до 1.
	float* arResVolatility = nullptr;

	// Настройки программы - не используем shared_ptr, так как к моменту вызова деструктора ~SettingsStorage графическая 
	// подсистема должна ещё существовать - а объект World глобальный и может умереть позже неё.
	SettingsStorage* pSettings;

	// Известные виды организмов. Древовидный список, в вершине - вид протоорганизма.
	std::shared_ptr<demi::Species> species;

	// Химические реакции, доступные для организмов.
	std::map<std::string, std::shared_ptr<demi::ChemReaction>> reactions;

	// Сами организмы.
	std::vector<demi::Organism*> animals;

	std::random_device random_device; // Источник энтропии.
	std::mt19937 generator; // Генератор случайных чисел.
	
	// Для генерации случайного числа. Направление движения при диффузии
	std::uniform_int_distribution<> rnd_angle;

	// Для генерации случайного числа. Приращение координаты при диффузии.
	std::uniform_int_distribution<> rnd_Coord;

	// Позиция протоорганизма для восстановления его каждый ход.
	clan::Pointf LUCAPos;

	// Обновить состояние.
	void makeTick();

	// Диффузия ресурсов.
	void diffusion();

	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	void fillRectResource(int resId, float amount, const clan::Rectf &rect);

	// Задаёт местоположение источников геотермальной энергии.
	void addGeothermal(int i, const clan::Pointf &coord);

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

	// Рекурсивная функция для считывания видов организмов.
	std::shared_ptr<demi::Species> doReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor);

	// Рекурсивная функция для записи видов организмов, параллельно создаёт словарь названий видов.
	void doWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies, std::set<std::string> &dict);

	// Вынесено из SaveModel() для удобства. Запись одного организма.
	void doWriteOrganism(clan::File &binFile, std::set<std::string> &dict, demi::Organism* organism);
	demi::Organism* doReadOrganism(clan::File &binFile, std::set<std::string> &dict, const clan::Pointf &center);
};




