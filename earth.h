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


// Продолжительность года в днях.
const int cDaysInYear = 365;

// Количество тиков в сутках, приравняем к секундам 60с*60м*24ч=86400
const int cTicksInDay = 86400;
//const int cTicksInDay = 1000;

// Глобальная переменная - указатель на мир.
class Earth;
extern Earth globalEarth;


//
// Точка на земной поверхности со списком ресурсов с указанием их количества.
//
class Dot {
public:
	
	// Солнечная энергия в точке.
	float solarEnergy = 0;

	// Геотермальная энергия в точке.
	float energy = 0;

	// Имеющиеся в ней ресурсы
	float *res;

	// Конструктор, массив ресов обнуляем.
	Dot();
	~Dot();

	// Возвращает цвет для точки на основе имеющихся ресурсов, передача по ссылке для оптимизации.
	void get_color(clan::Colorf &aValue) const;
};


//
// Локальные координаты - центр всегда в точке 0, 0 и можно адресовать отрицательные координаты.
//
class LocalCoord { 
public:
	// Центр локальных координат, выраженный в глобальных координатах.
	int xcenter, ycenter;

	LocalCoord(int x, int y) : xcenter(x), ycenter(y) {};

	Dot& operator()( int x, int y ) const;
};


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

	// Возвращает позицию для солнца в зависимости от дня года (по вертикали мира).
	unsigned int YPos(const DemiTime &timeModel);
	// Возвращает позицию для солнца в зависимости от времени суток (по горизонтали мира).
	unsigned int XPos(const DemiTime &timeModel);

public:

	// Облучает энергией для указанного момента времени землю.
	void Shine(const DemiTime &timeModel);
};


//
// Источник геотермальной энергии
//
class Geothermal {
public:
	unsigned int XPos;
	unsigned int YPos;
};


//
// Земная поверхность, двумерный массив точек.
//
class Earth {

	friend Dot;
	friend LocalCoord;

public:
	Earth();
	~Earth();

	// Считывает модель из xml-файла и сохраняет.
	void LoadModel(const std::string &filename);
	void SaveModel(const std::string &filename);

	// Приостанавливает или продолжает расчёт модели
	void RunEvolution(bool is_active);

	// Возвращает точки поверхности.
	Dot *getCopyDotsArray() { return arDotsCopy; }
	DemiTime getModelTime() { return timeBackup; }

	// Доступ к свойствам.
	int get_worldWidth() { return worldWidth; }
	int get_worldHeight() { return worldHeight; }
	int get_elemCount() { return elemCount; }
	int get_energyCount() { return energyCount; }
	int get_lightRadius() { return lightRadius; }
	int get_tropicHeight() { return tropicHeight; }

private:

	// Текущий момент времени в расчётной модели и в сохранённой для отрисовки на экране копии.
	DemiTime timeModel;
	DemiTime timeBackup;

	// Источник солнечной энергии.
	Solar solar;

	// Геотермальные источники - массив.
	Geothermal *arEnergy = nullptr;

	// Обновить состояние.
	void MakeTick();

	// Диффузия ресурсов.
	void Diffusion();


	// Размеры мира.
	int worldWidth = 0;
	int worldHeight = 0;

	// Количество химических элементов, существующих в модели.
	int elemCount = 0;

	// Количество геотермальных источников.
	int energyCount;

	// Максимальный радиус освещённости вокруг местонахождения солнца, составляет 90% высоты мира.
	// Эта же величина и максимальная яркость солнца - в самой удалённой точке она была минимальной и равной 1, соответственно в центре - равна радиусу.
	int lightRadius = 0;

	// Ход солнца по вертикали, обусловленный наклоном земной оси, примерно sin(23.5)=0.4 или по 10% с каждой стороны от экватора.
	int tropicHeight = 0;

	// Поверхность земли и копия для отображения на экране.
	Dot *arDots;
	Dot *arDotsCopy;

	// Цвета элементов.
	clan::Colorf* arResColors = nullptr;

	// Массив максимальных значений ресурсов для целей визуализации на экране.
	float *arResMax = nullptr;

	// Названия ресурсов. Инициализируется извне.
	std::string *arResNames = nullptr;
	
	// Летучесть ресурса для диффузии, значение от 0 до 1.
	float* arResVolatility = nullptr;

	// Рабочий поток
	std::thread thread;

	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	void FillRectResource(int resId, float amount, const clan::Rect &rect);

	// Задаёт местоположение источников геотермальной энергии.
	void AddGeothermal(int i, int x, int y);

private:

	bool thread_exit_flag = false;		// Если истина, поток должен завершиться.
	bool thread_run_flag = false;		// Если истина, то поток работает и изменяет модель, иначе простаивает.
	bool thread_crashed_flag = false;	// Если истина, значит поток завершился аварийно.
	//bool thread_complete_flag = false;	// Если истина, значит поток завершил расчёт модели (устанавливается в расчётном потоке для основного потока).
	std::mutex thread_mutex;
	std::condition_variable threadMainToWorkerEvent;
	//std::condition_variable threadWorkerToMainEvent;

	// Рабочая функция потока, вычисляющего модель.
	void worker_thread();
};




