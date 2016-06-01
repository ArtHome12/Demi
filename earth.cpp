/* ===============================================================================
	Моделирование эволюции живого мира.
	Модуль для основных компонентов - точка, пространство.
	24 august 2013.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "Earth.h"


const int cGeothermRadius = 12;

// Константы для чтения XML-файла модели
const std::string cResGlobalsEarthSize = "Globals/EarthSize";
const std::string cResGlobalsEarthSizeWidth = "width";
const std::string cResGlobalsEarthSizeHeightRatio = "heightRatio";

const std::string cResElementsSection = "Elements";
const std::string cResElementsType = "Element";

const std::string cResEnergySection = "Energy";
const std::string cResEnergyType = "Geothermal";

const std::string cResOrganismsSection = "Organisms";
const std::string cResOrganismsType = "Organism";

const std::string cResAreaRect = "rect";	// Тег для прямоугольной области начального распределения ресурса/организмов.
const std::string cResAreaPoint = "point";	// Тег для точки.

const std::string cElementsResColor = "color";
const std::string cElementsResVolatility = "volatility";

const std::string cSolar = "Solar";			// Обозначение солнечной энергии.
const std::string cEnergy = "Geothermal";			// Обозначение энергии.


// Глобальный объект - неживой мир.
Earth globalEarth;

// =============================================================================
// Точка на земной поверхности с координатами и списком ресурсов с указанием их количества.
// =============================================================================
Dot::Dot() 
{
	res = new float[globalEarth.get_elemCount()];
	memset(res, 0, sizeof(float) * globalEarth.get_elemCount());
}

Dot::~Dot()
{
	delete res;
}


void Dot::get_color(clan::Colorf &aValue) const
{
	// Возвращает цвет для точки на основе имеющихся ресурсов, передача по ссылке для оптимизации.
	//
	// Солнечный свет и энергия это альфа-канал.
	aValue.set_alpha(solarEnergy > energy ? solarEnergy : energy);

	// Если складываем вместе цвета разных элементов, возникают точки неожиданных цветов. 
	// Выходом может быть отображать преимущественный цвет для всей точки, то есть самый яркий. Начнём
	// с первого элемента и проверим остальные элементы.
	//

	// Вклад (возможная яркость) ресурса для первого элемента.
	float resBright = res[0] / globalEarth.arResMax[0];

	// Цвет текущего ресурса.
	clan::Colorf &col = globalEarth.arResColors[0];

	// Цвет ресурса, уменьшенный пропорционально вкладу.
	// Встроенный оператор изменяет 4 компоненты, поэтому для оптимизации делаем это вручную.
	//
	aValue.x = col.x;
	aValue.y = col.y;
	aValue.z = col.z;

	// В цикле проверим яркости остальных элементов.
	//
	for (int i = 1; i < globalEarth.elemCount; ++i) {

		// Вклад текущего ресурса.
		const float curResBright = res[i] / globalEarth.arResMax[i];

		// Если яркость выше, запоминаем цвет.
		//
		if (resBright < curResBright) {
			resBright = curResBright;
			clan::Colorf &col = globalEarth.arResColors[i];
			aValue.x = col.x;
			aValue.y = col.y;
			aValue.z = col.z;
		}
	}
}

// =============================================================================
// Локальные координаты - центр всегда в точке 0, 0 и можно адресовать отрицательные координаты.
// =============================================================================
Dot& LocalCoord::operator()( int x, int y ) const 
{
	// По горизонтали координату переносим с одного края на другой.
	//
	x += xcenter;
	if (x < 0)
		x += globalEarth.worldWidth;
	else if (x >= globalEarth.worldWidth)
		x -= globalEarth.worldWidth;

	// По вертикали пока просто отрезаем.
	//
	y += ycenter;
	if (y < 0) 
		y = 0;
	else if (y >= globalEarth.worldHeight)
		y = globalEarth.worldHeight - 1;

	_ASSERT(x < globalEarth.worldWidth);
	_ASSERT(y < globalEarth.worldHeight);

	return globalEarth.arDots[x + y * globalEarth.worldWidth];
}

// =============================================================================
// Формат мирового времени
// =============================================================================
void DemiTime::MakeTick()
{
	// Прибавляет один тик.
	//
	// Прибавляем одну секунду и проверяем, не достигли ли полных суток
	if (++sec >= cTicksInDay) {

		// Обнуляем секунды, прибавляем один день и аналогично секундам.
		sec = 0;
		if (++day >= cDaysInYear) {
			day = 1;

			// Года просто прибавляем до скончания времён.
			year++;
		}
	}
}

std::string DemiTime::getDateStr() const
{
	// Возвращает строку с временем модели.
	//
	return clan::string_format("%1:%2:%3", year, day, sec);
}


// =============================================================================
// Источник солнечной энергии
// =============================================================================
void Solar::Shine(const DemiTime &timeModel)
{
	// Облучает энергией для указанного момента времени землю.
	//
	// Сброс старой освещённости должен был быть сделан при перемешивании, но практически она затухает сама по мере удаления от центра.

	// Определим центр солнца.
	//
	int cx = XPos(timeModel);
	int cy = YPos(timeModel);

	// Определим систему координат.
	LocalCoord coord(cx, cy);

	// Двигаемся по всем точкам
	//
	int lightRadius = globalEarth.get_lightRadius();
	for (int x = -lightRadius; x <= lightRadius; x++)
		for (int y = -lightRadius; y <= lightRadius; y++) {

			// Фактическое расстояние до центра координат.
			int r = int(sqrt(x*x + y*y));

			// Если оно больше заданного радиуса, ничего не делаем, иначе зададим освещённость, обратную расстоянию в долях от 0 до 1.
			coord(x, y).solarEnergy = r < lightRadius ? float(lightRadius - r) / lightRadius : 0;
		}

}


unsigned int Solar::YPos(const DemiTime &timeModel)
{
	// Возвращает позицию для солнца в зависимости от дня года (по вертикали мира).
	//
	// Солнце полгода двигается от одной границы тропиков до другой и полгода в обратном направлении, то есть через полгода позиция повторяется.
	//
	
	// Половина года, для удобства.
	const int halfYear = cDaysInYear / 2;

	// Когда идёт первая половина года, надо от экватора отнимать долю, а когда вторая - прибавлять.
	//
	if (timeModel.day < halfYear)
		return (globalEarth.get_worldHeight() - globalEarth.get_tropicHeight()) / 2 + timeModel.day * globalEarth.get_tropicHeight() / halfYear;
	return (globalEarth.get_worldHeight() + globalEarth.get_tropicHeight()) / 2 - (timeModel.day - halfYear) * globalEarth.get_tropicHeight() / halfYear;
}


unsigned int Solar::XPos(const DemiTime &timeModel)
{
	// Возвращает позицию для солнца в зависимости от времени суток (по горизонтали мира).
	//
	// Солнце двигается с востока на запад пропорционально прошедшей доле суток.
	return unsigned int(globalEarth.get_worldWidth() * (1.0f - float(timeModel.sec - 1) / (cTicksInDay - 1)));
}


// =============================================================================
// Земная поверхность, двумерный массив точек.
// =============================================================================
Earth::Earth()
{
	// Инициализируем генератор случайных чисел.
	srand((unsigned)::time(NULL));

	// Создадим поток. Он не стартанёт до установки флага.
	thread = std::thread(&Earth::worker_thread, this);
}



Earth::~Earth()
{
	// Завершим работу потока, если он упал раньше, то код всё равно выполняется корректно.
	//
	std::unique_lock<std::mutex> lock(thread_mutex);
	thread_exit_flag = true;
	lock.unlock();
	threadMainToWorkerEvent.notify_all();
	thread.join();

	delete[] arDots;
	delete[] arResColors;
	delete[] arResMax;
	delete[] arResNames;
	delete[] arResVolatility;
	delete[] arEnergy;
}


void Earth::MakeTick()
{
	// Обновить состояние.
	//
	// Обновляем время мира.
	timeModel.MakeTick();

	// Облучаем мир солнечной энергией.
	solar.Shine(timeModel);

	// Перемешивание ресурсов из-за диффузии.
	Diffusion();
}



void Earth::FillRectResource(int resId, float amount, const clan::Rect &rect)
{
	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	//
	// Откорректируем максимум, если требуется.
	//
	if (arResMax[resId] < amount)
		arResMax[resId] = amount;

	for (int x = rect.left; x < rect.right; ++x)
		for (int y = rect.top; y < rect.bottom; ++y)
			arDots[x + y * worldWidth].res[resId] = amount;
}


void Earth::Diffusion()
{
	// Диффузия ресурсов.
	//
	// Граница массива, на которую заходить нельзя.
	Dot *cur = arDots;	// Исходная точка для переноса ресурсов - первая точка массива.
	Dot *last = cur + worldWidth * worldHeight;	// Точка после последней точки массива.
	Dot *dest;			// Конечная точка

	while (true) {

		// Случайное число.
		int random = rand();
		int rnd = random % 7; // от 0 до 7, направление движения
		int rndRes = random % elemCount; // случайный элемент.

		// Определяем исходную координату отдельным случайным числом, иначе возникает корреляция между начальной точкой и направлением.
		cur += rand() % 12;

		// Если прсмотрели все точки, прерываемся.
		if (cur >= last)
			break;

		// Определяем конечную координату - в одну из сторон от исходной.
		switch (rnd) {
		case 1 :
			dest = cur + 1;					// на восток.
			break;
		case 2 :
			dest = cur + 1 + worldWidth;	// на юго-восток.
			break;
		case 3 :
			dest = cur + worldWidth;		// на юг.
			break;
		case 4 :
			dest = cur - 1 + worldWidth;	// на юго-запад.
			break;
		case 5 :
			dest = cur - 1;					// на запад.
			break;
		case 6 :
			dest = cur - 1 - worldWidth;	// на северо-запад.
			break;
		default:
			dest = cur - worldWidth;		// на север.
		}
		
		// Откорректируем в случае выхода за пределы, иначе всё вещество скопится у нижнего края из-за дрейфа
		// в силу того, что перенесённое вперёд вещество снова участвует в переносе.
		//
		if (dest < arDots)
			dest = last - (arDots - dest);
		else if (dest >= last)
			dest = arDots + (dest - last);

		// Осуществим перенос вещества из исходной точки в конечную согласно летучести ресурса.
		//
		Dot &fromDot = *cur;
		Dot &toDot = *dest;
		const float amount = fromDot.res[rndRes] * arResVolatility[rndRes];
		fromDot.res[rndRes] -= amount;
		toDot.res[rndRes] += amount;

		// Обновим максимумы.
		//
		if (arResMax[rndRes] < toDot.res[rndRes])
			arResMax[rndRes] = toDot.res[rndRes];
	}
}


void Earth::AddGeothermal(int i, int x, int y)
{
	// Задаёт местоположение источников геотермальной энергии.
	//

	arEnergy[i].XPos = x;
	arEnergy[i].YPos = y;

	// Так как геотермальные источники на данный момент считаем неизменяемыми, сразу же зададим распределение энергии.
	//

	// Определим систему координат.
	LocalCoord coord(x, y);

	for (int xp = -cGeothermRadius; xp <= cGeothermRadius; xp++)
		for (int yp = -cGeothermRadius; yp <= cGeothermRadius; yp++) {

			// Фактическое расстояние до центра координат.
			int r = int(sqrt(xp*xp + yp*yp));

			// Если оно больше заданного радиуса, ничего не делаем, иначе зададим освещённость, обратную расстоянию в долях от 0 до 1.
			if (r < cGeothermRadius) 
				coord(xp, yp).energy = float(cGeothermRadius - r) / cGeothermRadius;
		}
}


void Earth::LoadModel(const std::string &filename)
{
	// Считывает модель из xml-файла
	//
	// Если поток был запущен, сообщим об ошибке.
	{
		std::unique_lock<std::mutex> lock(thread_mutex);
		if (thread_run_flag)
			throw clan::Exception("Need to stop the evolution prior to load!");
	}

	// Откроем XML файл.
	auto resDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// Инициализируем размеры мира.
	//
	const clan::DomElement &prop = resDoc->get_resource(cResGlobalsEarthSize).get_element();

	// Ширина мира.
	worldWidth = prop.get_attribute_int(cResGlobalsEarthSizeWidth);

	// Высота мира
	worldHeight = int(worldWidth * prop.get_attribute_float(cResGlobalsEarthSizeHeightRatio));

	if (worldWidth <= 0 || worldWidth > 30000 || worldHeight <= 0 || worldHeight > 30000)
		throw clan::Exception("Invalid world size (width=" + clan::StringHelp::int_to_text(worldWidth) + ", height=" + clan::StringHelp::int_to_text(worldHeight) + ")");

	// Радиус солнечного пятна и высота тропиков зависит от размера мира.
	lightRadius = int(0.9 * worldHeight / 2);
	tropicHeight = int(worldHeight / 5);

	// Названия ресурсов.
	const std::vector<std::string>& names = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);

	// Количество ресурсов.
	elemCount = int(names.size());

	// Источники энергии.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);

	// Количество источников.
	energyCount = int(energy.size());

	// Выделим память под массивы.
	//
	arDots = new Dot[worldWidth * worldHeight];		// точки поверхности.
	arResColors = new clan::Colorf[elemCount];
	arResMax = new float[elemCount];
	arResNames = new std::string[elemCount];
	arResVolatility = new float[elemCount];
	arEnergy = new Geothermal[energyCount];			// геотермальные источники.

	// Инициализируем массив максимумов единицами, во-избежание деления на ноль.
	//
	for (int i = 0; i < elemCount; ++i)
		arResMax[i] = 1;


	//
	// Считываем названия элементов.
	//
	for (int i = 0; i < elemCount; ++i) {

		// Элемент.
		clan::XMLResourceNode &res = resDoc->get_resource(names[i]);

		// Название элемента.
		arResNames[i] = res.get_name();

		// Свойства элемента.
		clan::DomElement &prop = res.get_element();

		// Цвет для отображения.
		arResColors[i] = clan::Colorf(prop.get_attribute(cElementsResColor));

		// Летучесть, значение от 0 до 1.
		arResVolatility[i] = prop.get_attribute_float(cElementsResVolatility);

		// Области для заполнения ресурсом.
		//
		// Ищем прямоугольные области и задаём распределение ресурса.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaRect);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// Надйенный элемент с информацией о распределении ресурса.
			clan::DomElement &rectItem = nodes.item(j).to_element();

			int r = rectItem.get_attribute_int("right");
			if (r == 0)
				r = worldWidth;

			int b = rectItem.get_attribute_int("bottom");
			if (b == 0)
				b = worldHeight;

			// Область.
			clan::Rect rect(rectItem.get_attribute_int("left"), rectItem.get_attribute_int("top"), r, b);

			// Заполняем точки.
			FillRectResource(i, rectItem.get_attribute_float("amount"), rect);
		}
	}


	// Считываем местоположение геотермальных источников.
	//
	for (int i = 0; i < energyCount; ++i) {
		clan::DomElement &prop = resDoc->get_resource(energy[i]).get_element();
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaPoint);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// Надйенный элемент с информацией о распределении ресурса.
			clan::DomElement &pointItem = nodes.item(j).to_element();

			// Заполняем точки.
			AddGeothermal(i, pointItem.get_attribute_int("x"), pointItem.get_attribute_int("y"));
		}
	}
}


void Earth::SaveModel(const std::string &filename)
{
	// Сохраняет модель в xml-файл
	//
	// XML-файл не сохраняем, так как он не меняется моделью.
}


void Earth::worker_thread()
{
	// Рабочая функция потока, вычисляющего модель.
	//
	try
	{
		while (true)
		{
			// Останавливаемся до установки флагов, блокируя при необходимости основной поток.
			std::unique_lock<std::mutex> lock(thread_mutex);
			threadMainToWorkerEvent.wait(lock, [&]() { return thread_run_flag || thread_exit_flag; });

			// Если дана команда на выход, завершаем работу.
			if (thread_exit_flag)
				break;

			// Сбрасываем флаг наличия результата.
			//thread_complete_flag = false;

			// Разблокируем основной поток.
			lock.unlock();

			// Выполняем расчёт модели.
			MakeTick();

			// Блокируем основной поток для сохранения результата.
			lock.lock();

			// Копируем расчётную модель в буфер, используемый для отображения.
			timeBackup = timeModel;

			// Сообщаем о наличии результата.
			//thread_complete_flag = true;
			//threadWorkerToMainEvent.notify_all();


			//throw clan::Exception("Bang!");	// <--- Use this to test the application handles exceptions in threads
			// Тут основной поток разблокируется при завершении блока {}
		}
	}
	catch (clan::Exception &)
	{
		// Сообщим об аварийном завершении работы. Текст ошибки допишем потом.
		std::unique_lock<std::mutex> lock(thread_mutex);
		thread_crashed_flag = true;
	}
}

void Earth::RunEvolution(bool is_active)
{
	// Приостанавливает или продолжает расчёт модели
	//
	std::unique_lock<std::mutex> lock(thread_mutex);
	thread_run_flag = is_active;
	threadMainToWorkerEvent.notify_all();
}


//Dot * Earth::getCopyDotsArray()
//{
//	// Приостанавливает расчёт модели, копирует мир в копию и снова запускает расчёт модели.
//	//
//
//	// Если расчётный поток запущен, надо подождать до получения результата.
//	if (thread_run_flag) {
//		// Останавливаемся до установки флага результата, блокируя при необходимости расчётный поток.
//		std::unique_lock<std::mutex> lock(thread_mutex);
//		threadWorkerToMainEvent.wait(lock, [&]() { return thread_complete_flag; });
//
//		// Возвращаем результат.
//		return arDotsCopy;
//
//		// Разблокируем расчётный поток при выходе из блока {}.
//	}
//	else {
//		// Если расчётный поток не работает, можно сразу копировать результат.
//		return arDotsCopy;
//	}
//
//	return arDotsCopy;
//}
