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
#include "local_coord.h"
#include "genotypes_tree.h"
#include "organism.h"
#include "settings_storage.h"
#include "reactions.h"


const size_t cGeothermRadius = 12;

// Константы для чтения XML-файла модели
auto cResGlobalsWorldSize = "Globals/WorldSize";
auto cResGlobalsWorldSizeWidth = "width";
auto cResGlobalsWorldSizeHeightRatio = "heightRatio";
auto cResGlobalsWorldResolution = "resolution";

auto cResGlobalsAppearance = "Globals/Appearance";
auto cResGlobalsAppearanceTop = "top";
auto cResGlobalsAppearanceLeft = "left";
auto cResGlobalsAppearanceScale = "scale";

auto cResGlobalsMetabolicConsts = "Globals/MetabolicConsts";
auto cResGlobalsMCminActiveMetabolicRate = "minActiveMetabolicRate";
auto cResGlobalsMCminInactiveMetabolicRate = "minInactiveMetabolicRate";
auto cResGlobalsMCdesintegrationVitalityBarrier = "desintegrationVitalityBarrier";

auto cResGlobalsLUCA = "Globals/LUCA";
auto cResGlobalsLUCAMutationChance = "mutationChance";
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

auto cResGenesSection = "Genes";				// Синхронно с cResReactionsSection!
auto cResGenesType = "Gene";
auto cResGenesValue = "Value";
auto cResGenesValueName = "name";

auto cResReactionsSection = "Genes/Reaction";	// Синхронно с cResGenesSection!
auto cResReactionsGeoEnergy = "geothermalEnergy";
auto cResReactionsSolarEnergy = "solarEnergy";
auto cResReactionsVitalityProductivity = "vitalityProductivity";
auto cResReactionsLeftReagent = "LeftReagent";
auto cResReactionsRightReagent = "RightReagent";
auto cResReactionsReagentName = "name";
auto cResReactionsAmount = "amount";


auto cResGenotypesSection = "Genotypes";
auto cResGenotypesType = "Genotype";
auto cResGenotypesName = "name";
auto cResGenotypesAuthor = "author";
auto cResGenotypesAliveColor = "aliveColor";
auto cResGenotypesDeadColor = "deadColor";
auto cResGenotypesGeneName = "geneName";

auto cResAreaRect = "rect";	// Тег для прямоугольной области начального распределения ресурса/организмов.
auto cResAreaPoint = "point";	// Тег для точки.

auto cSolar = "Solar";			// Обозначение солнечной энергии.
auto cEnergy = "Geothermal";	// Обозначение геотермальной энергии.

// Строковые ресурсы
auto cWrongSize = "WorldWrongSize";
auto cWrongResolution = "WorldWrongResolution";
auto cWrongReaction = "WrongReaction";
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
	demi::LocalCoord coord(getPos(timeModel));

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
	const clan::Size& worldSize = globalWorld.getWorldSize();
	 
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
World::World() : generator(random_device()), rnd_angle(0, 7), rnd_Coord(0, 12 - 1), thread(std::thread(&World::workerThread, this)), genotypesTree(std::make_shared<demi::GenotypesTree>())
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

	// Перебор всех точек и вычисления.
	processDots();

	// Создаём экземпляр протоорганизма, если нет живых организмов и есть место.
	if (!genotypesTree->genotype->getAliveCount()) {
		demi::Dot& protoDot = demi::LocalCoord(LUCAPos).get_dot(0, 0);
		if (protoDot.organism == nullptr)
			new demi::Organism(LUCAPos, 0, 1, timeModel, 0, genotypesTree->species.front());
	}
	//runEvolution(false);
}


void World::fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect)
{
	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	//
	for (size_t x = rect.left; x != rect.right; ++x)
		for (size_t y = rect.top; y != rect.bottom; ++y)
			arDots[getDotIndexFromXY(x, y)].setElemAmount(resId, amount);
}


// Перебор всех точек и вычисления.
void World::processDots()
{
	// Значения массива arResMax, используемые для выбора цвета точки по самому относительно концентрированному элементу в ней, не должны всегда только расти.
	// С какой-то периодичностью надо их актуализировать.Если делать это при сохранении, либо при загрузке, либо время от времени, то появится некрасивый эффект скачкообразного изменения внешнего вида без соответствующего
	// изменения модели.Если модель перед сохранением выглядела одним образом, а после стала выглядеть иначе, это будет воспринято как баг. Поэтому просто при каждом тике максимальные концентрации снижаем на единицу.
	for (size_t i = 0; i != elemCount; ++i)
		if (arResMax[i])
			--arResMax[i];


	// Границы массива, за которую заходить нельзя.
	demi::Dot *cur = arDots;											// Исходная точка для переноса ресурсов - первая точка массива.
	demi::Dot *last = cur + worldSize.width * worldSize.height;		// Точка после последней точки массива.
	demi::Dot *dest;													// Конечная точка

	// Элемент
	std::uniform_int_distribution<> rnd_Elem(0, elemCount-1);

	while (true) {

		// Определяем исходную координату отдельным случайным числом.
		cur += rnd_Coord(generator);

		// Если прсмотрели все точки, прерываемся.
		if (cur >= last)
			break;

		// Определяем конечную координату - в одну из сторон от исходной.
		size_t rnd = rnd_angle(generator);						// от 0 до 7, направление движения
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
		demi::Dot &fromDot = *cur;
		demi::Dot &toDot = *dest;
		const size_t rndResIndex = rnd_Elem(generator);			// случайный элемент.
		unsigned long long amount = unsigned long long(fromDot.res[rndResIndex] * arResVolatility[rndResIndex] + 0.5f);

		// В ситуации, когда вещества в точке мало и перемножение на летучесть даёт ноль, переместим 1 единицу, иначе оно останется неподвижным навечно в отсутствие живых организмов.
		if (!amount && fromDot.res[rndResIndex])
			amount = 1;

		fromDot.res[rndResIndex] -= amount;
		toDot.res[rndResIndex] += amount;

		// Обработка организма, если он есть. Таким образом у организмов приоритет - в то время как в неживой природе меняется один элемент, меняется и организм.
		// Перед доступом к полям организма делаем однократную попытку получить монопольный доступ к нему. 
		// Если не удалось, переходим к следующей точке, а ему не повезло (альтернатива - ждать освобождения). 
		// С одной стороны такое поведение ускоряет расчёт модели, с другой - потоки-просмотрщики начинают влиять на модель, замедляя организмы, на которые смотрит пользователь.
		// С третьей стороны это добавляет нелинейности в эволюцию и на фоне непрерывных случайных чисел влияние возможно ничтожно.
		demi::Organism* curOrganism = fromDot.organism;
		if (curOrganism && curOrganism->tryLock()) {

			// Если мёртвый организм распался и его надо удалить, сделаем это.
			if (curOrganism->needDesintegration()) {
				// Из текущей точки организм удалит сам себя в деструкторе.
				delete curOrganism;
			}
			else {
				
				// Передаём организму управление и определяем его желание размножаться.
				if (curOrganism->makeTick()) {

					// Пытаемся найти подходящее свободное место.
					clan::Point freePlace;
					if (curOrganism->findFreePlace(freePlace)) {

						// Ополовиниваем жизненную силу у исходного организма (будет передана дочернему).
						int32_t newVitality = curOrganism->halveVitality();

						// от 0 до 7, направление поворота нового организма.
						uint8_t newAngle = rnd_angle(generator);

						// Вид нового организма, возможно с мутацией либо тот же самый.
						const std::shared_ptr<demi::Species>& oldSpec = curOrganism->getSpecies();
						std::shared_ptr<demi::GenotypesTree> treeNode = oldSpec->getGenotype()->getTreeNode();
						const std::shared_ptr<demi::Species> newSpec = treeNode->breeding(oldSpec);

						// Количество делений с защитой от переполнения.
						uint64_t newAncestorCount = curOrganism->getAncestorsCount() + 1;
						if (newAncestorCount == UINT64_MAX)
							--newAncestorCount;

						// Если была мутация, то сбросим счётчик предыдущих делений, а также обновим окно видов.
						if (oldSpec != newSpec)
							newAncestorCount = 0;

						// Местоположение нового организма переведём в глобальные координаты.
						clan::Point newPoint = curOrganism->getCenter().getGlobalPoint(freePlace);

						// Создаём производный организм. По новому местоположению он поместит сам себя в конструкторе.
						new demi::Organism(newPoint, newAngle, newVitality, timeModel, newAncestorCount, newSpec);
					}
				}
				
				// Если в конечной точке нет организма, то попытаемся перенести исходный.
				if (toDot.organism == nullptr && curOrganism->canMove()) {
					// Координаты новой точки.
					curOrganism->moveTo(getDotXYFromIndex(dest - arDots));

					// Уменьшаем жизненную энергию, потраченную на перенос.
					curOrganism->processInactiveVitality();
				}
				else {
					// Если организм мёртв, то уменьшаем его энергию, чтобы он распался.
					if (!curOrganism->isAlive())
						curOrganism->processInactiveVitality();
				}

				// Разблокируем организм.
				curOrganism->unlock();
			}
		}

		// Обновим максимумы.
		//
		if (arResMax[rndResIndex] < toDot.res[rndResIndex])
			arResMax[rndResIndex] = toDot.res[rndResIndex];
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
	demi::LocalCoord geothermalCoord(coord);
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

	// Загрузим настройки и неживую природу.
	doLoadInanimal(resDoc);

	// Загрузим реакции.
	doLoadReactions(resDoc);

	// Загрузим живую природу.
	doLoadAnimal(resDoc);

	// Загрузим двоичный файл.
	doLoadBinary(filename);

	// Инициализирует массив максимумов на основе имеющихся количеств в точках, используется после загрузки.
	InitResMaxArray();

	// Инициализируем объект для подсчёта количества элементов неживой природы и организмов разных видов.
	amounts.Init();
}

// Для разгрузки функций loadModel, saveModel.
void World::doLoadInanimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
{
	// Инициализируем размеры мира.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// Ширина мира.
	worldSize.width = prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000);

	// Высота мира
	worldSize.height = int(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f) + 0.5f);
	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongSize), worldSize.width, worldSize.height));

	// Разрешающая способность, считываем в процентах и переводим в шаг с количеством точек от 1 до 100.
	int resolution = prop.get_attribute_int(cResGlobalsWorldResolution);
	if (resolution == 0 || resolution > 100)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongResolution), resolution));

	resolution = int(100.0f / resolution + 0.5f);
	rnd_Coord = std::uniform_int_distribution<>(0, resolution);

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

	// Источники энергии.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);
	energyCount = energy.size();

	// Элементы.
	const std::vector<std::string>& elementNames = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);
	elemCount = elementNames.size();

	// Выделим память под массивы.
	arDots = new demi::Dot[worldSize.width * worldSize.height];		// точки поверхности.
	arResColors = new clan::Color[elemCount];					// цвета элементов.
	arResMax = new unsigned long long[elemCount];				// максимальные концентрации элементов в одной точке.
	arResNames = new std::string[elemCount];					// названия элементов.
	arResVisible = new bool[elemCount];							// Видимость элементов.
	arResVolatility = new float[elemCount];						// летучесть элементов.
	arEnergy = new Geothermal[energyCount];						// геотермальные источники.

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
}

void World::doLoadReactions(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
{
	// Считываем химические реакции.
	reactions.clear();

	// Получим нужный узел с реакциями.
	clan::XMLResourceNode &res = resDoc->get_resource(cResReactionsSection);

	// Перебираем все значения, то есть все реакции.
	clan::DomNodeList& reactionNodes = res.get_element().get_elements_by_tag_name(cResGenesValue);
	for (int i = reactionNodes.get_length() - 1; i >= 0; --i) {

		// Текущий узел с информацией о реакции.
		clan::DomElement &reactionNode = reactionNodes.item(i).to_element();

		// Создаём реакцию.
		auto curReaction = std::make_shared<demi::ChemReaction>();

		// Поля реакции - название, количество необходимой гео и солнечной энергии и выхлоп жизненной энергии.
		curReaction->name = reactionNode.get_attribute(cResGenesValueName);
		curReaction->geoEnergy = reactionNode.get_attribute_float(cResReactionsGeoEnergy);
		curReaction->solarEnergy = reactionNode.get_attribute_float(cResReactionsSolarEnergy);
		curReaction->vitalityProductivity = reactionNode.get_attribute_int(cResReactionsVitalityProductivity);

		// Реагенты слева.
		clan::DomNodeList& leftNodes = reactionNode.get_elements_by_tag_name(cResReactionsLeftReagent);
		for (int i = leftNodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = leftNodes.item(i).to_element();

			// Название (элемента) реагента.
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// Создаём реагент с индексом вместо названия и количеством.
			demi::ReactionReagent reagent((uint8_t)findElemIndex(reagentName), node.get_attribute_int(cResReactionsAmount));

			// Добавляем реагент в реакцию.
			curReaction->leftReagents.push_back(reagent);
		}

		// Реагенты справа.
		clan::DomNodeList& rightNodes = reactionNode.get_elements_by_tag_name(cResReactionsRightReagent);
		for (int i = rightNodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = rightNodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");
			demi::ReactionReagent reagent((uint8_t)findElemIndex(reagentName), node.get_attribute_int(cResReactionsAmount));
			curReaction->rightReagents.push_back(reagent);
		}

		// Сохраняем реакцию в списке реакций.
		reactions.push_back(curReaction);
	}
}


void World::doLoadAnimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
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
	clan::Color specAliveColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCAAliveColor)));
	clan::Color specDeadColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCADeadColor)));
	LUCAPos = clan::Point(prop.get_attribute_int("x"), prop.get_attribute_int("y"));
	
	// Константа с вероятностью мутации, доля единицы.
	float mutationChance = prop.get_attribute_float(cResGlobalsLUCAMutationChance);

	// Создадим такой генератор, чтобы в нём было количество значений, определёных разрядом mutationChance.
	rnd_Mutation = std::uniform_int_distribution<>(0, int(1.0 / mutationChance));

	const std::string LUCAGeneName = prop.get_attribute("geneName");
	const std::string LUCAReactionName = prop.get_attribute("geneValue");

	// Очистим старое дерево генотипов/видов. Может быть переопределено при считывании двоичного файла.
	genotypesTree->clear();

	// Прочитаем раздел с генами.
	//
	// Список всех названий генов.
	const std::vector<std::string>& geneNames = resDoc->get_resource_names_of_type(cResGenesType, cResGenesSection);

	// Временное хранилище под данные генов (востребованные будут скопированы в генотипы).
	std::map<std::string, std::vector<std::string>> geneDatas;

	// Перебираем все разделы с генами
	for (auto& geneName : geneNames) {

		// Структуры для разбора XML-файла.
		clan::XMLResourceNode &resGene = resDoc->get_resource(geneName);
		clan::DomElement &propGene = resGene.get_element();

		// Вектор под варианты значений.
		std::vector<std::string> tmpVect;

		// Содержимое подразделов - перечень значений.
		clan::DomNodeList& nodes = propGene.get_elements_by_tag_name(cResGenesValue);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			// Сохраняем название в векторе.
		