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
#include "world.h"


const size_t cGeothermRadius = 12;

// Константы для чтения XML-файла модели
auto cResGlobalsWorldSize = "Globals/WorldSize";
auto cResGlobalsWorldSizeWidth = "width";
auto cResGlobalsWorldSizeHeightRatio = "heightRatio";

auto cResGlobalsAppearance = "Globals/Appearance";
auto cResGlobalsAppearanceTop = "top";
auto cResGlobalsAppearanceLeft = "left";
auto cResGlobalsAppearanceScale = "scale";

auto cResGlobalsMetabolicConsts = "Globals/MetabolicConsts";
auto cResGlobalsMCminActiveMetabolicRate = "minActiveMetabolicRate";
auto cResGlobalsMCminInactiveMetabolicRate = "minInactiveMetabolicRate";
auto cResGlobalsMCdesintegrationVitalityBarrier = "desintegrationVitalityBarrier";

auto cResGlobalsLUCA = "Globals/LUCA";
auto cResGlobalsLUCAVisibility = "visibility";
auto cResGlobalsLUCAFissionBarier = "fissionBarrier";
auto cResGlobalsLUCAAliveColor = "aliveColor";
auto cResGlobalsLUCADeadColor = "deadColor";

auto cResGlobalsTime = "Globals/Time";
auto cResGlobalsTimeYear = "year";
auto cResGlobalsTimeDay = "day";
auto cResGlobalsTimeSecond = "second";

auto cResElementsSection = "Elements";
auto cResElementsType = "Element";
auto cResElementsColor = "color";
auto cResElementsVolatility = "volatility";
auto cResElementsVisibility = "visibility";

auto cResEnergySection = "Energy";
auto cResEnergyType = "Geothermal";

auto cResReactionsSection = "Reactions";
auto cResReactionsType = "Reaction";
auto cResReactionsGeoEnergy = "geothermalEnergy";
auto cResReactionsSolarEnergy = "solarEnergy";
auto cResReactionsVitalityProductivity = "vitalityProductivity";
auto cResReactionsLeftReagent = "LeftReagent";
auto cResReactionsRightReagent = "RightReagent";
auto cResReactionsReagentName = "name";
auto cResReactionsAmount = "amount";


auto cResOrganismsSection = "Organisms";

auto cResAreaRect = "rect";	// Тег для прямоугольной области начального распределения ресурса/организмов.
auto cResAreaPoint = "point";	// Тег для точки.

auto cSolar = "Solar";			// Обозначение солнечной энергии.
auto cEnergy = "Geothermal";	// Обозначение геотермальной энергии.

// Строковые ресурсы
auto cWrongSize = "WorldWrongSize";
auto cWrongLUCAReaction = "WrongLUCAReaction";
auto cWrongBinVer = "WorldWrongBinaryFileVersion";
auto cWrongBinElemCount = "WorldWrongBinaryFileElementsCount";
auto cWrongBinElemName = "WorldWrongBinaryFileElementsName";
auto cWrongBinMarker = "WorldWrongBinaryFileMarker";
auto cWrongBinCannotReadDots = "WorldWrongBinaryFileCannotReadDots";

// Глобальный объект - неживой мир.
World globalWorld;

// =============================================================================
// Источник солнечной энергии
// =============================================================================
void Solar::Shine(const demi::DemiTime &timeModel)
{
	// Облучает энергией для указанного момента времени землю.
	//
	// Сброс старой освещённости должен был быть сделан при перемешивании, но практически она затирается сама.

	// Определим систему координат с положением солнца в центре.
	LocalCoord coord(getPos(timeModel));

	int lightRadius = int(globalWorld.getLightRadius());
	for (int x = -lightRadius + 1; x != lightRadius; ++x)			// Симметрично отбрасываем координаты самую левую (верхнюю) и самую правую (нижнюю) для ускорения - иначе ради 1 точки проходить весь ряд прямоугольника.
		for (int y = -lightRadius + 1; y != lightRadius; ++y) {

			// Фактическое расстояние до центра координат.
			double r = sqrt(x*x + y*y);

			// Если оно больше заданного радиуса, ничего не делаем, иначе зададим освещённость, обратную расстоянию в долях от 0 до 1.
			r = r < lightRadius ? float((lightRadius - r) / lightRadius) : 0.0f;
			
			coord.get_dot(x, y).setSolarEnergy(float(r));
		}
}


clan::Point Solar::getPos(const demi::DemiTime &timeModel)
{
	// Возвращает позицию для солнца в зависимости от времени.

	// Размеры мира.
	const clan::Size &worldSize = globalWorld.get_worldSize();

	// Позиция по горизонтали зависит от времени суток.
	// Солнце двигается с востока на запад пропорционально прошедшей доле суток.
	int x = int(worldSize.width * (1.0f - float(timeModel.sec) / (demi::cTicksInDay - 1.0f)) + 0.5f);

	// Позиция по вертикали зависит от дня года.
	// Солнце полгода двигается от одной границы тропиков до другой и полгода в обратном направлении, 
	// то есть через полгода позиция повторяется.
	
	// Половина года, для удобства.
	const int halfYear = int(demi::cDaysInYear / 2 + 0.5f);

	// Высота тропиков
	const int tropic = int(globalWorld.getTropicHeight());

	// Когда идёт первая половина года, надо от экватора отнимать долю, а когда вторая - прибавлять.
	int y = int((int(timeModel.day) < halfYear ? (worldSize.height - tropic) / 2.0f + int(timeModel.day) * tropic / halfYear : (worldSize.height + tropic) / 2.0 - (int(timeModel.day) - halfYear) * tropic / halfYear) +0.5);

	return clan::Point(x, y);
}



// =============================================================================
// Земная поверхность, двумерный массив точек.
// =============================================================================
World::World() : generator(random_device()), rnd_angle(0, 7), rnd_Coord(0, 12 - 1), thread(std::thread(&World::workerThread, this))

{
	// Поток не стартанёт до установки флага.
}



World::~World()
{
	// Завершим работу потока, если он упал раньше, то код всё равно выполняется корректно.
	std::unique_lock<std::mutex> lock(threadMutex);
	threadExitFlag = true;
	lock.unlock();
	threadEvent.notify_all();
	thread.join();

	delete[] arDots;
	delete[] arResColors;
	delete[] arResMax;
	delete[] arResNames;
	delete[] arResVisible;
	delete[] arResVolatility;
	delete[] arEnergy;
}


void World::makeTick()
{
	// Обновить состояние.
	//
	// Обновляем время мира.
	timeModel.MakeTick();

	// Облучаем мир солнечной энергией.
	solar.Shine(timeModel);

	// Перемешивание ресурсов из-за диффузии.
	diffusion();

	// Передаём управление живым организмам.
	//
	
	// Надо оценить эффективность данного способа, мож через случайное число будет быстрее.
	std::shuffle(animals.begin(), animals.end(), generator);

	// Используем традиционый цикл, так как иначе итераторы портятся после изменения вектора.
	size_t cnt = animals.size();
	for (size_t i = 0; i < cnt; ++i) {

		demi::Organism &animal = *animals[i];

		// Проверим, если организм мертв, его надо удалить из списка.
		if (!animal.isAlive()) {
			// Меняем его с последним и удаляем последний.
			animals[i] = animals.back();
			animals.pop_back();
			--cnt;
			continue;
		}

		// Передаём ему управление. Если обратно получили не 0, надо сохранить новый организм в списке.
		demi::Organism * newBorn = animal.makeTickAndGetNewBorn();
		if (newBorn != nullptr) {
			animals.push_back(newBorn);
			++cnt;
		}
	}

	// Создаём экземпляр протоорганизма, если нет живых организмов и есть место.
	if (!cnt) {
		Dot& protoDot = LocalCoord(LUCAPos).get_dot(0, 0);
		if (protoDot.organism == nullptr)
			animals.push_back(new demi::Organism(LUCAPos, 0, 1, getModelTime(), 0, genotypesTree.species.front()));
	}
}



void World::fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect)
{
	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	//
	for (size_t x = rect.left; x != rect.right; ++x)
		for (size_t y = rect.top; y != rect.bottom; ++y)
			arDots[getDotIndexFromXY(x, y)].setElementAmount(resId, amount);
}


// Диффузия ресурсов.
void World::diffusion()
{
	// Значения массива arResMax, используемые для выбора цвета точки по самому относительно концентрированному элементу в ней, не должны всегда только расти.
	// С какой-то периодичностью надо их актуализировать.Если делать это при сохранении, либо при загрузке, либо время от времени, то появится некрасивый эффект скачкообразного изменения внешнего вида без соответствующего
	// изменения модели.Если модель перед сохранением выглядела одним образом, а после стала выглядеть иначе, это будет воспринято как баг. Поэтому просто при каждом тике максимальные концентрации снижаем на единицу.
	for (size_t i = 0; i != elemCount; ++i)
		if (arResMax[i])
			--arResMax[i];


	// Границы массива, за которую заходить нельзя.
	Dot *cur = arDots;											// Исходная точка для переноса ресурсов - первая точка массива.
	Dot *last = cur + worldSize.width * worldSize.height;		// Точка после последней точки массива.
	Dot *dest;													// Конечная точка

	// Элемент
	std::uniform_int_distribution<> rnd_Elem(0, elemCount-1);

	while (true) {

		size_t rnd = rnd_angle(generator);			// от 0 до 7, направление движения
		const size_t rndResIndex = rnd_Elem(generator);		// случайный элемент.

		// Определяем исходную координату отдельным случайным числом.
		cur += rnd_Coord(generator);

		// Если прсмотрели все точки, прерываемся.
		if (cur >= last)
			break;

		// Определяем конечную координату - в одну из сторон от исходной.
		switch (rnd) {
		case 0:	// на восток.
			dest = cur + 1;
			break;
		case 1: // на юго-восток.
			dest = cur + 1 + worldSize.width;
			break;
		case 2:	// на юг.
			dest = cur + worldSize.width;
			break;
		case 3:	// на юго-запад.
			dest = cur - 1 + worldSize.width;
			break;
		case 4:	// на запад.
			dest = cur - 1;
			break;
		case 5:	// на северо-запад.
			dest = cur - 1 - worldSize.width;
			break;
		case 6:	// на север.
			dest = cur - worldSize.width;
			break;
		default:	// на северо-восток.
			dest = cur + 1 - worldSize.width;
		}
		
		// Откорректируем в случае выхода за пределы, иначе всё вещество скопится у нижнего края из-за дрейфа
		// в силу того, что перенесённое вперёд вещество снова участвует в переносе.
		if (dest < arDots)
			dest = last - (arDots - dest);
		else if (dest >= last)
			dest = arDots + (dest - last);

		// Осуществим перенос вещества из исходной точки в конечную согласно летучести ресурса.
		Dot &fromDot = *cur;
		Dot &toDot = *dest;
		unsigned long long amount = unsigned long long(fromDot.res[rndResIndex] * arResVolatility[rndResIndex] + 0.5f);

		// В ситуации, когда вещества в точке мало и перемножение на летучесть даёт ноль, переместим 1 единицу, иначе оно останется неподвижным навечно в отсутствие живых организмов.
		if (!amount && fromDot.res[rndResIndex])
			amount = 1;

		fromDot.res[rndResIndex] -= amount;
		toDot.res[rndResIndex] += amount;

		// Обновим максимумы.
		//
		if (arResMax[rndResIndex] < toDot.res[rndResIndex])
			arResMax[rndResIndex] = toDot.res[rndResIndex];

		// Обработка организма.
		demi::Organism* curOrganism = fromDot.organism;
		if (!curOrganism)
			continue;

		// Если в исходной точке есть организм и его надо удалить, сделаем это.
		if (curOrganism->needDesintegration()) {
			// Из текущей точки организм удалит сам себя в деструкторе.
			delete curOrganism;
		}
		else {
			// Если в конечной точке нет организма, то попытаемся перенести исходный.
			if (toDot.organism == nullptr && curOrganism->canMove()) {
				// Координаты новой точки.
				curOrganism->moveTo(getDotXYFromIndex(dest - arDots));

				// Уменьшаем жизненную энергию, потраченную на перенос.
				curOrganism->processInactiveVitality();
			} else {
				// Если организм мёртв, то уменьшаем его энергию, чтобы он распался.
				if (!curOrganism->isAlive())
					curOrganism->processInactiveVitality();
			}
		}
	}
}

// Возвращает координаты точки по указанному индексу.
clan::Point World::getDotXYFromIndex(size_t index)
{
	int y = int(index / worldSize.width);
	int x = index - y * worldSize.width;
	return clan::Point(x, y);
}


void World::addGeothermal(size_t i, const clan::Point &coord)
{
	// Задаёт местоположение источников геотермальной энергии.
	//

	arEnergy[i].coord = coord;

	// Так как геотермальные источники на данный момент считаем неизменяемыми, сразу же зададим распределение энергии.
	//

	// Определим систему координат.
	LocalCoord geothermalCoord(coord);
	int rad = cGeothermRadius;
	for (int xp = -rad; xp <= rad; ++xp)
		for (int yp = -rad; yp <= rad; ++yp) {

			// Фактическое расстояние до центра координат.
			double r = sqrt(xp*xp + yp*yp);

			// Если оно больше заданного радиуса, ничего не делаем, иначе зададим освещённость, обратную расстоянию в долях от 0 до 1.
			if (r < rad)
				geothermalCoord.get_dot(xp, yp).setGeothermalEnergy(float((rad - r) / rad));
		}
}


void World::loadModel(const std::string &filename)
{
	// Считывает модель из xml-файла

	// Если поток был запущен, сообщим об ошибке.
	{
		std::unique_lock<std::mutex> lock(threadMutex);
		if (threadRunFlag)
			throw clan::Exception(pSettings->LocaleStr("WorldLoadModelErrorNeedToStop"));
	}

	// Откроем XML файл.
	auto resDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// Ключи в секциях для оптимизации считаем один раз.
	const std::vector<std::string>& elementNames = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);
	const std::vector<std::string>& reactionNames = resDoc->get_resource_names_of_type(cResReactionsType, cResReactionsSection);

	// Загрузим настройки и неживую природу.
	doLoadInanimal(resDoc, elementNames, reactionNames);

	// Загрузим живую природу.
	doLoadAnimal(resDoc, filename, reactionNames);

	// Инициализируем объект для подсчёта количества элементов неживой природы и организмов разных видов.
	amounts.Init();
}

// Для разгрузки функций loadModel, saveModel.
void World::doLoadInanimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc, const std::vector<std::string>& elementNames, const std::vector<std::string>& reactionNames)
{
	// Инициализируем размеры мира.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// Ширина мира.
	worldSize.width = prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000);

	// Высота мира
	worldSize.height = int(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f) + 0.5f);
	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongSize), worldSize.width, worldSize.height));

	// Радиус солнечного пятна и высота тропиков зависит от размера мира.
	lightRadius = size_t(0.9f * worldSize.height / 2 + 0.5f);
	tropicHeight = size_t(0.2f * worldSize.height + 0.5f);

	// Инициализируем внешний вид проекта.
	prop = resDoc->get_resource(cResGlobalsAppearance).get_element();
	appearanceTopLeft.x = prop.get_attribute_int(cResGlobalsAppearanceLeft, appearanceTopLeft.x);
	appearanceTopLeft.y = prop.get_attribute_int(cResGlobalsAppearanceTop, appearanceTopLeft.y);
	appearanceScale = prop.get_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// Прочитаем время.
	prop = resDoc->get_resource(cResGlobalsTime).get_element();
	timeModel.year = prop.get_attribute_int(cResGlobalsTimeYear, 1);
	timeModel.day = prop.get_attribute_int(cResGlobalsTimeDay, 1);
	timeModel.sec = prop.get_attribute_int(cResGlobalsTimeSecond, 0);
	timeBackup = timeModel;

	// Источники энергии.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);

	// Количество источников.
	energyCount = energy.size();

	// Количество ресурсов.
	elemCount = elementNames.size();

	// Выделим память под массивы.
	arDots = new Dot[worldSize.width * worldSize.height];	// точки поверхности.
	arResColors = new clan::Color[elemCount];				// цвета элементов.
	arResMax = new unsigned long long[elemCount];			// максимальные концентрации элементов в одной точке.
	arResNames = new std::string[elemCount];				// названия элементов.
	arResVisible = new bool[elemCount];						// Видимость элементов.
	arResVolatility = new float[elemCount];					// летучесть элементов.
	arEnergy = new Geothermal[energyCount];					// геотермальные источники.

															// Считываем названия элементов.
	for (size_t i = 0; i != elemCount; ++i) {

		// Элемент.
		clan::XMLResourceNode &res = resDoc->get_resource(elementNames[i]);

		// Название элемента.
		arResNames[i] = res.get_name();

		// Свойства элемента.
		clan::DomElement &prop = res.get_element();

		// Цвет для отображения. У версии Color нет почему-то поиска по имени, приходится через преобразование.
		arResColors[i] = clan::Color(clan::Colorf(prop.get_attribute(cResElementsColor)));

		// Летучесть, значение от 0 до 1.
		arResVolatility[i] = prop.get_attribute_float(cResElementsVolatility);

		// Видимость элемента.
		arResVisible[i] = prop.get_attribute_bool(cResElementsVisibility, true);

		// Области для заполнения ресурсом.

		// Ищем прямоугольные области и задаём распределение ресурса.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaRect);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// Найденный элемент с информацией о распределении ресурса.
			clan::DomElement &rectItem = nodes.item(j).to_element();

			int r = rectItem.get_attribute_int("right");
			if (!r)
				r = worldSize.width;

			int b = rectItem.get_attribute_int("bottom");
			if (!b)
				b = worldSize.height;

			// Область.
			clan::Rect rect(rectItem.get_attribute_int("left"), rectItem.get_attribute_int("top"), r, b);

			// Заполняем точки.
			fillRectResource(i, rectItem.get_attribute_int("amount"), rect);
		}
	}

	// Считываем местоположение геотермальных источников.
	for (size_t i = 0; i != energyCount; ++i) {
		clan::DomElement &prop = resDoc->get_resource(energy[i]).get_element();
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaPoint);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// Найденный элемент с информацией о распределении ресурса.
			clan::DomElement &pointItem = nodes.item(j).to_element();

			// Заполняем точки.
			addGeothermal(i, clan::Point(pointItem.get_attribute_int("x"), pointItem.get_attribute_int("y")));
		}
	}

	// Считываем химические реакции.
	reactions.clear();
	for (auto & reactionName : reactionNames) {
		// Создаём реакцию.
		auto curReaction = std::make_shared<demi::ChemReaction>();

		// Количество необходимой энергии и выхлоп жизненной энергии.
		clan::XMLResourceNode &res = resDoc->get_resource(reactionName);
		clan::DomElement &prop = res.get_element();
		curReaction->name = res.get_name();
		curReaction->geoEnergy = prop.get_attribute_float(cResReactionsGeoEnergy);
		curReaction->solarEnergy = prop.get_attribute_float(cResReactionsSolarEnergy);
		curReaction->vitalityProductivity = prop.get_attribute_int(cResReactionsVitalityProductivity);

		// Реагенты слева.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResReactionsLeftReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// По названию получаем индекс элемента и добавляем его в реакцию вместе с количеством.
			auto pos = std::distance(elementNames.begin(), find(elementNames.begin(), elementNames.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->leftReagents.push_back(reagent);
		}

		// Реагенты справа.
		nodes = prop.get_elements_by_tag_name(cResReactionsRightReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// По названию получаем индекс элемента и добавляем его в реакцию вместе с количеством.
			auto pos = std::distance(elementNames.begin(), find(elementNames.begin(), elementNames.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->rightReagents.push_back(reagent);
		}

		// Сохраняем реакцию в списке реакций.
		reactions.push_back(curReaction);
	}

	// Инициализирует массим максимумов на основе имеющихся количеств в точках, используется после загрузки.
	InitResMaxArray();
}


void World::doLoadAnimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc, const std::string &filename, const std::vector<std::string>& reactionNames)
{
	// Создаём вид протоорганизма.
	//
	// Считываем константы для организмов.
	auto prop = resDoc->get_resource(cResGlobalsMetabolicConsts).get_element();
	demi::Organism::minActiveMetabolicRate = uint8_t(prop.get_attribute_int(cResGlobalsMCminActiveMetabolicRate));
	demi::Organism::minInactiveMetabolicRate = uint8_t(prop.get_attribute_int(cResGlobalsMCminInactiveMetabolicRate));
	demi::Organism::desintegrationVitalityBarrier = int32_t(prop.get_attribute_int(cResGlobalsMCdesintegrationVitalityBarrier));

	// Считываем вид протоорганизма.
	prop = resDoc->get_resource(cResGlobalsLUCA).get_element();
	bool specVisible = prop.get_attribute_bool(cResGlobalsLUCAVisibility);
	//uint16_t specFissionBarrier = prop.get_attribute_int(cResGlobalsLUCAFissionBarier);
	clan::Color specAliveColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCAAliveColor)));
	clan::Color specDeadColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCADeadColor)));
	LUCAPos = clan::Point(prop.get_attribute_int("x"), prop.get_attribute_int("y"));
	const std::string LUCAGeneName = prop.get_attribute("geneName");
	const std::string LUCAReactionName = prop.get_attribute("geneValue");

	// Определим индекс реакции протоорганизма (учитывая что названия реакций идут с префиксом секции).
	auto itReaction = std::find(reactionNames.begin(), reactionNames.end(), std::string(cResReactionsSection) + "/" + LUCAReactionName);
	if (itReaction == reactionNames.end())
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongLUCAReaction), LUCAReactionName));
	demi::geneValues_t geneValue = demi::geneValues_t(std::distance(reactionNames.begin(), itReaction));

	// Очистим старое дерево генотипов/видов.
	genotypesTree.clear();

	// Создаём ген реакций.
	demi::Gene gene(LUCAGeneName, reactionNames);

	// Создаём корневой генотип и присваиваем ему первый индекс.
	genotypesTree.genotype = std::make_shared<demi::Genotype>(nullptr, gene, "LUCA", "Demi");

	// Создаём вид специальным конструктором для протоорганизма.
	auto species = std::make_shared<demi::Species>(genotypesTree.genotype, geneValue, specVisible, specAliveColor, specDeadColor);
	species->getCellsRef().push_back(std::make_shared<demi::CellAbdomen>());

	// Сохраняем корневой вид в дереве.
	genotypesTree.species.push_back(species);

	// Перед двоичным файлом удалим прежние организмы, если они были.
	animals.clear();

	// Считываем двоичный файл, если он есть.
	if (clan::FileHelp::file_exists(filename + "b")) {

		clan::File binFile(filename + "b",				// У двоичного файла расширение будет 'demib'.
			clan::File::OpenMode::open_existing,
			clan::File::AccessFlags::access_read,
			clan::File::ShareFlags::share_read);

		// Версия файла.
		auto &strVer = binFile.read_string_nul();
		if (strVer != "Ver:1")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinVer), strVer));

		// Количество элементов.
		auto &strElemCount = binFile.read_string_nul();
		auto &strElemCountAwait = "ElementsCount:" + clan::StringHelp::int_to_text(int(elemCount));
		if (strElemCount != strElemCountAwait)
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemCount), strElemCountAwait, strElemCount));

		// Названия элементов.
		for (size_t i = 0; i != elemCount; ++i) {
			auto &elemName = binFile.read_string_nul();
			if (elemName != arResNames[i])
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemName), arResNames[i], elemName));
		}

		// Создадим при помощи дерева словарь видов для того, чтобы при считывании организмов использовать лишь ключ словаря.
		// Пока всего один экземпляр генотипов.
		speciesDict_t speciesDict;
		genotypesTree.generateDict(speciesDict);

		// Маркер начала массива точек.
		auto strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Dots:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Dots:", strSecMarker));

		// Считываем точки неживого мира. Клетки будут размещены при считывании организмов.
		size_t dotsCount = worldSize.width * worldSize.height;
		for (size_t i = 0; i != dotsCount; ++i) {

			// Текущая точка.
			Dot &dot = arDots[i];

			// Количества элементов в точке.
			const size_t elemArraySize = sizeof(uint64_t) * elemCount;
			if (binFile.read(dot.res, elemArraySize) != elemArraySize)
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadDots), i));

			// Организм в точке.
			dot.organism = doReadOrganism(binFile, speciesDict, getDotXYFromIndex(i));
		}

		// Закроем файл.
		binFile.close();
	}
}


/*// Рекурсивная функция для считывания видов организмов. 
std::shared_ptr<demi::Species> World::doReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor)
{
	// Считываем текущий вид.

	// Создаём вид и инициализиуем основные поля.
	std::shared_ptr<demi::Species> retVal = std::make_shared<demi::Species>();
	retVal->ancestor = ancestor;
	retVal->name = binFile.read_string_nul();
	retVal->author = binFile.read_string_nul();
	retVal->visible = binFile.read_int8() != 0;
	unsigned char r = binFile.read_uint8(), g = binFile.read_uint8(), b = binFile.read_uint8();
	retVal->aliveColor = clan::Color(r, g, b);
	r = binFile.read_uint8(); g = binFile.read_uint8(); b = binFile.read_uint8();
	retVal->deadColor = clan::Color(r, g, b);
	
	// Метаболитическая реакция.
	std::string reactionName = binFile.read_string_nul();
	retVal->reaction = reactions[reactionName];

	// Начальный порог размножения (будет меняться из-за изменчивости).
	retVal->fissionBarrier = binFile.read_uint32();

	// Создаём и считываем клетки.

	// Количество клеток.
	uint64_t cnt = binFile.read_uint64();
	
	// В цикле создаём все клетки.
	for (int i = 0; i < cnt; ++i) {

		// Тип клетки.
		uint64_t cellType = binFile.read_uint64();

		// Создаём клетку нужного типа.
		std::shared_ptr<demi::GenericCell> cell;
		switch (cellType) {
		case 0 :	// cellBrain мозг
			break;
		case 1:		// cellReceptor рецептор
			break;
		case 2:		// cellMuscle мышца
			break;
		case 3:		// cellAdipose жир
			break;
		case 4:		// cellAbdomen живот
			cell = std::make_shared<demi::CellAbdomen>();
			break;
		case 5:		// cellMouth рот
			break;
		case 6:		// cellArmor броня
			break;
		}

		// Добавляем клетку в вид организма.
		retVal->cells.push_back(cell);
	}


	// Считываем дочерние виды.

	// Количество дочерних видов.
	cnt = binFile.read_uint64();

	// Считываем и сохраняем потомков рекурсивно.
	for (uint64_t i = 0; i != cnt; ++i)
		retVal->descendants.push_back(doReadSpecies(binFile, retVal));
	
	// Возвращаем считанный элемент.
	return retVal;
}

// Рекурсивная функция для записи видов организмов, параллельно создаёт словарь названий видов.
void World::doWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies, std::set<std::string> &dict)
{
	// Записываем основные поля.
	binFile.write_string_nul(aSpecies->getName());
	binFile.write_string_nul(aSpecies->getAuthor());
	binFile.write_int8(aSpecies->getVisible());

	// Цвета отображения.
	const clan::Color& aliveColor = aSpecies->getAliveColor();
	const clan::Color& deadColor = aSpecies->getDeadColor();
	binFile.write_uint8(aliveColor.get_red());
	binFile.write_uint8(aliveColor.get_green());
	binFile.write_uint8(aliveColor.get_blue());
	binFile.write_uint8(deadColor.get_red());
	binFile.write_uint8(deadColor.get_green());
	binFile.write_uint8(deadColor.get_blue());

	binFile.write_string_nul(aSpecies->reaction->name);

	// Сохраняем название в словаре.
	dict.insert(aSpecies->getAuthorAndNamePair());

	// Начальный порог размножения.
	binFile.write_uint32(aSpecies->fissionBarrier);

	// Записываем клетки.
	binFile.write_uint64(aSpecies->cells.size());
	for (auto& cell : aSpecies->cells) {
		// Тип клетки.
		uint64_t cellType = cell->getCellType();
		binFile.write_uint64(cellType);
	}

	// Записываем дочерние виды.

	// Количество дочерних видов.
	binFile.write_uint64(aSpecies->descendants.size());

	// Сами виды рекурсивно.
	for (auto &spec : aSpecies->descendants)
		doWriteSpecies(binFile, spec, dict);
}*/


void World::saveModel(const std::string &filename)
{
	// Сохраняет модель

	// Если поток был запущен, надо его приостановить.
	std::unique_lock<std::mutex> lock(threadMutex);
	bool prevRun = threadRunFlag;
	
	if (threadRunFlag) {
		// Сигнал на остановку.
		threadRunFlag = false;
		threadEvent.notify_all();

		// Ждём остановки.
		do {
			lock.unlock();
			clan::System::sleep(100);
			lock.lock();
		} while (threadRunFlag);
	}

	// Настройки, хранимые в XML-файле.
	auto pResDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// Сохраняем внешний вид проекта.
	clan::DomElement &prop = pResDoc->get_resource(cResGlobalsAppearance).get_element();
	prop.set_attribute_int(cResGlobalsAppearanceLeft, appearanceTopLeft.x);
	prop.set_attribute_int(cResGlobalsAppearanceTop, appearanceTopLeft.y);
	prop.set_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// Запишем видимость протоорганизма.
	prop = pResDoc->get_resource(cResGlobalsLUCA).get_element();
	prop.set_attribute_bool(cResGlobalsLUCAVisibility, genotypesTree.species.front()->getVisible());

	// Записываем время.
	prop = pResDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, int(timeModel.year));
	prop.set_attribute_int(cResGlobalsTimeDay, int(timeModel.day));
	prop.set_attribute_int(cResGlobalsTimeSecond, int(timeModel.sec));

	// Запишем видимость элементов.
	for (size_t i = 0; i != elemCount; ++i) {

		// Элемент.
		prop = pResDoc->get_resource(cResElementsSection + std::string("/") + arResNames[i]).get_element();

		// Видимость элемента.
		prop.set_attribute_bool(cResElementsVisibility, arResVisible[i]);
	}

	// Записываем изменения на диск.
	pResDoc->save(filename);
	
	// Двоичный файл под точки, организмы и т.д.
	clan::File binFile(filename + "b",				// У двоичного файла расширение будет 'demib'.
		clan::File::OpenMode::create_always,
		clan::File::AccessFlags::access_write,
		clan::File::ShareFlags::share_read);

	// Запишем версию файла.
	binFile.write_string_nul("Ver:1");
	
	// Запишем количество элементов.
	binFile.write_string_nul("ElementsCount:" + clan::StringHelp::int_to_text(int(elemCount)));

	// Запишем названия элементов.
	for (size_t i = 0; i != elemCount; ++i)
		binFile.write_string_nul(arResNames[i]);

	// Для того, чтобы не писать перед каждым организмом строку с его названием генотипа, сделаем словарь и будем пользоваться номером.
	speciesDict_t speciesDict;
	genotypesTree.generateDict(speciesDict);

	// Записываем маркер начала массива точек.
	binFile.write_string_nul("Dots:");

	// Записываем точки.
	size_t dotsCount = worldSize.width * worldSize.height;
	const size_t elemArraySize = sizeof(uint64_t) * elemCount;
	for (size_t i = 0; i != dotsCount; ++i) {
		// Текущая точка.
		const Dot &dot = arDots[i];

		// Количества элементов в точке.
		binFile.write(dot.res, elemArraySize);

		// Организм в точке.
		doWriteOrganism(binFile, speciesDict, dot.organism);
	}

	// Закроем файл.
	binFile.close();

	// Продолжим выполнение потока, если он работал.
	if (prevRun) {
		threadRunFlag = true;
		threadEvent.notify_all();
	}
}


void World::workerThread()
{
	// Рабочая функция потока, вычисляющего модель.
	try
	{
		while (true)
		{
			// Останавливаемся до установки флагов, блокируя при необходимости основной поток.
			std::unique_lock<std::mutex> lock(threadMutex);
			threadEvent.wait(lock, [&]() { return threadRunFlag || threadExitFlag; });

			// Если дана команда на выход, завершаем работу.
			if (threadExitFlag)
				break;

			// Разблокируем основной поток.
			lock.unlock();

			// Выполняем расчёт модели.
			makeTick();

			// Блокируем основной поток для сохранения результата.
			lock.lock();

			// Копируем расчётную модель в буфер, используемый для отображения.
			timeBackup = timeModel;

			//throw clan::Exception("Bang!");	// <--- Use this to test the application handles exceptions in threads

			// Тут основной поток разблокируется при завершении блока {}
		}
	}
	catch (clan::Exception &)
	{
		// Сообщим об аварийном завершении работы. Текст ошибки допишем потом.
		std::unique_lock<std::mutex> lock(threadMutex);
		threadCrashedFlag = true;
	}
}

void World::runEvolution(bool is_active)
{
	// Приостанавливает или продолжает расчёт модели
	std::unique_lock<std::mutex> lock(threadMutex);
	threadRunFlag = is_active;
	threadEvent.notify_all();
}

// Начинает расчёт заново.
void World::resetModel(const std::string &modelFilename, const std::string &defaultFilename)
{
	// Удаляем двоичный файл.
	if (clan::FileHelp::file_exists(modelFilename + "b"))
		clan::FileHelp::delete_file(modelFilename + "b");

	// Попытаемся загрузить модель.
	try {
		globalWorld.loadModel(modelFilename);
		timeModel = demi::DemiTime();
		timeBackup = timeModel;
	}
	catch (...) {
		// При ошибке загружаем чистую модель.
		globalWorld.loadModel(defaultFilename);
	}
}

// Вынесено из SaveModel() для удобства. Запись одного организма.
void World::doWriteOrganism(clan::File &binFile, speciesDict_t& speciesDict, demi::Organism* organism)
{
	// Если организма нет, в качестве ключа вида запишем UINT16_MAX.
	if (!organism) {
		binFile.write_uint16(UINT16_MAX);
		return;
	}

	// Находим указатель на вид организма в словаре и записываем его индекс. Проверку на наличие вида в словаре не делаем.
	auto it = std::find(speciesDict.begin(), speciesDict.end(), organism->getSpecies());
	ptrdiff_t index = std::distance(speciesDict.begin(), it);
	binFile.write_uint16(uint16_t(index));

	// Записываем поля вида (значения генов, видимость и цвета).
	organism->getSpecies()->saveToFile(binFile);

	// Записываем собственные поля организма.
	organism->saveToFile(binFile);
}

demi::Organism* World::doReadOrganism(clan::File &binFile, speciesDict_t& speciesDict, const clan::Point &center)
{
	uint16_t dictKey = binFile.read_uint16();

	// Если в качестве ключа UINT16_MAX, значит организма в точке нет.
	if (dictKey == UINT16_MAX)
		return nullptr;

	// Указатель на вид получим по индексу из словаря.
	auto it = speciesDict.begin();
	std::advance(it, dictKey);
	const std::shared_ptr<demi::Species>& species = *it;

	// Создадим организм из файла.
	demi::Organism* retVal = demi::Organism::createFromFile(binFile, center, species);

	// Если жизненная энергия положительна, поместим организм в список живых.
	if (retVal->isAlive())
		animals.push_back(retVal);

	return retVal;
}

// Инициализирует массим максимумов на основе имеющихся количеств в точках, используется после загрузки.
void World::InitResMaxArray()
{
	// Очистим старые значения.
	memset(arResMax, 0, sizeof(unsigned long long) * elemCount);

	// Перебираем все точки и сохраняем количества.
	Dot *cur = globalWorld.getDotsArray();						// Первая точка массива.
	Dot *last = cur + worldSize.width * worldSize.height;		// Точка после последней точки массива.
	while (cur < last) {

		// Перебираем все элементы в точке.
		for (size_t j = 0; j != elemCount; ++j) {
			
			// Количество вещества в точке.
			unsigned long long amnt = cur->getElemAmount(j);

			// Корректируем при необходимости максимум.
			if (getResMaxValue(j) < amnt)
				arResMax[j] = amnt;
		}

		++cur;
	}
}
