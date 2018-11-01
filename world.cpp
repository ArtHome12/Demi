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
			tmpVect.push_back(node.get_attribute(cResGenesValueName, ""));
		}

		// Сохраняем ген в хранилище, в том числе имя без префикса.
		geneDatas.insert(std::pair<std::string, std::vector<std::string>>(resGene.get_name(), tmpVect));
	}

	// Создаём ген протоорганизма (ген реакций) по данным из карты. Текущий временный объект будет скопирован в генотипе, оверхедом пренебрегаем.
	demi::Gene gene(LUCAGeneName, geneDatas.at(LUCAGeneName));

	// Создаём генотип протоорганизма.
	genotypesTree->genotype = std::make_shared<demi::Genotype>(genotypesTree, gene, "LUCA", "Demi", specAliveColor, specDeadColor);

	// Определим индекс реакции протоорганизма, то есть значение его гена.
	// Для этого переберём все реакции в поисках нужной. По мере увеличения количества можно переделать на map.
	geneValues_t geneValue = geneValues_t_MAX;
	for (size_t i = 0; i != reactions.size(); ++i)
		if (reactions[i]->name == LUCAReactionName) {
			geneValue = (geneValues_t)i;
			break;
		}
	if (geneValue == geneValues_t_MAX)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongLUCAReaction), LUCAReactionName));

	// Создаём вид специальным конструктором для протоорганизма. Может быть переопределён при считывании двоичного файла.
	auto species = std::make_shared<demi::Species>(genotypesTree->genotype, geneValue, true);

	// Сохраняем корневой вид в дереве.
	genotypesTree->species.push_back(species);

	// Загрузим рекурсивно остальные генотипы.
	auto genotypeNames = resDoc->get_resource_names_of_type(cResGenotypesType, cResGenotypesSection);
	for (auto& genotypeName : genotypeNames) {
		doLoadGenotypes(resDoc->get_resource(genotypeName).get_element(), genotypesTree, geneDatas);
	}
}

void World::doLoadGenotypes(clan::DomElement& node, std::shared_ptr<demi::GenotypesTree> ancestorTreeItem, std::map<std::string, std::vector<std::string>>& geneDatas)
{
	// Поля - название, автор, цвета, название гена и его значение.
	std::string name = node.get_attribute(cResGenotypesName);
	std::string author = node.get_attribute(cResGenotypesAuthor);
	clan::Color aliveColor = clan::Color(clan::Colorf(node.get_attribute(cResGenotypesAliveColor)));
	clan::Color deadColor = clan::Color(clan::Colorf(node.get_attribute(cResGenotypesDeadColor)));
	std::string geneName = node.get_attribute(cResGenotypesGeneName);

	// Создаём узел дерева под новый генотип и сохраняем его в дереве.
	std::shared_ptr<demi::GenotypesTree> newGenotypeTreeNode = std::make_shared<demi::GenotypesTree>();
	newGenotypeTreeNode->ancestor = ancestorTreeItem;
	ancestorTreeItem->derivatives.push_back(newGenotypeTreeNode);

	// Создаём ген протоорганизма (ген реакций) по данным из карты. Текущий временный объект будет скопирован в генотипе, оверхедом пренебрегаем.
	demi::Gene gene(geneName, geneDatas.at(geneName));

	// Создаём генотип.
	std::shared_ptr<demi::Genotype> newGenotype = std::make_shared<demi::Genotype>(newGenotypeTreeNode, gene, name, author, aliveColor, deadColor);

	// Сохраняем генотип в дереве.
	newGenotypeTreeNode->genotype = newGenotype;

	// Рекурсивно вызываем себя для вложенных генотипов.
	clan::DomNodeList& nodes = node.get_elements_by_tag_name(cResGenotypesType);
	for (int i = nodes.get_length() - 1; i >= 0; --i) {
		doLoadGenotypes(nodes.item(i).to_element(), newGenotypeTreeNode, geneDatas);
	}



	// Создаём новый генотип.
}


void World::doLoadBinary(const std::string &filename)
{
	// Считываем двоичный файл, если он есть.
	if (clan::FileHelp::file_exists(filename + "b")) {

		// Файл с диска считываем в память для ускорения разбора. У двоичного файла расширение будет 'demib'.
		clan::DataBuffer data = clan::File::read_bytes(filename + "b");
		clan::MemoryDevice binFile(data);

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

		// Маркер начала видов организмов.
		auto strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Species:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Species:", strSecMarker));

		// Считываем поля производных видов (значения генов, видимость и цвета), удалив вид протоорганизма.
		genotypesTree->clear();
		genotypesTree->loadFromFile(binFile);
			
		// Создадим при помощи дерева словарь видов для того, чтобы при считывании организмов использовать лишь ключ словаря.
		speciesDict_t speciesDict;
		genotypesTree->generateDict(speciesDict);

		// Маркер начала массива точек.
		strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Dots:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Dots:", strSecMarker));

		// Считываем точки неживого мира. Клетки будут размещены при считывании организмов.
		size_t dotsCount = worldSize.width * worldSize.height;
		for (size_t i = 0; i != dotsCount; ++i) {

			// Текущая точка.
			demi::Dot &dot = arDots[i];

			// Количества элементов в точке.
			const size_t elemArraySize = sizeof(uint64_t) * elemCount;
			if (binFile.read(dot.res, elemArraySize) != elemArraySize)
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadDots), i));

			// Организм в точке.
			dot.organism = doReadOrganism(binFile, speciesDict, getDotXYFromIndex(i));
		}
	}
}


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

	// Сохраняем изменения в XML-структуре.
	doSaveSettings(pResDoc);

	// Записываем изменения на диск.
	pResDoc->save(filename);

	// Записываем двоичный файл.
	doSaveBinary(filename);

	// Продолжим выполнение потока, если он работал.
	if (prevRun) {
		threadRunFlag = true;
		threadEvent.notify_all();
	}
}


void World::doSaveSettings(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
{
	// Сохраняем внешний вид проекта.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsAppearance).get_element();
	prop.set_attribute_int(cResGlobalsAppearanceLeft, appearanceTopLeft.x);
	prop.set_attribute_int(cResGlobalsAppearanceTop, appearanceTopLeft.y);
	prop.set_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// Записываем время.
	prop = resDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, int(timeModel.year));
	prop.set_attribute_int(cResGlobalsTimeDay, int(timeModel.day));
	prop.set_attribute_int(cResGlobalsTimeSecond, int(timeModel.sec));

	// Запишем видимость элементов.
	for (size_t i = 0; i != elemCount; ++i) {

		// Элемент.
		prop = resDoc->get_resource(cResElementsSection + std::string("/") + arResNames[i]).get_element();

		// Видимость элемента.
		prop.set_attribute_bool(cResElementsVisibility, arResVisible[i]);
	}
}

void World::doSaveBinary(const std::string &filename)
{
	// Создаём буфер в памяти для ускорения сохранения.
	clan::MemoryDevice binFile;

	// Запишем версию файла.
	binFile.write_string_nul("Ver:1");

	// Запишем количество элементов.
	binFile.write_string_nul("ElementsCount:" + clan::StringHelp::int_to_text(int(elemCount)));

	// Запишем названия элементов.
	for (size_t i = 0; i != elemCount; ++i)
		binFile.write_string_nul(arResNames[i]);

	// Записываем маркер начала видов организмов и поля видов.
	binFile.write_string_nul("Species:");

	// Записываем поля видов (значения генов, видимость и цвета).
	genotypesTree->saveToFile(binFile);

	// Для того, чтобы не писать перед каждым организмом строку с его названием генотипа, сделаем словарь и будем пользоваться номером.
	speciesDict_t speciesDict;
	genotypesTree->generateDict(speciesDict);

	// Записываем маркер начала массива точек.
	binFile.write_string_nul("Dots:");

	// Записываем точки.
	size_t dotsCount = worldSize.width * worldSize.height;
	const size_t elemArraySize = sizeof(uint64_t) * elemCount;
	for (size_t i = 0; i != dotsCount; ++i) {
		// Текущая точка.
		const demi::Dot &dot = arDots[i];

		// Количества элементов в точке.
		binFile.write(dot.res, elemArraySize);

		// Организм в точке.
		doWriteOrganism(binFile, speciesDict, dot.organism);
	}

	// Записываем буфер в файл. У двоичного файла расширение будет 'demib'.
	clan::File::write_bytes(filename + "b", binFile.get_data());
}


void World::workerThread()
{
	// Рабочая функция потока, вычисляющего модель.
	try
	{
		while (true)
		{
			// Останавливаемся до установки флагов, блокируя при необходимости основной (интерфейсный) поток.
			std::unique_lock<std::mutex> lock(threadMutex);
			threadEvent.wait(lock, [&]() { return threadRunFlag || threadExitFlag; });

			// Если дана команда на выход, завершаем работу.
			if (threadExitFlag)
				break;

			// Разблокируем основной поток.
			lock.unlock();

			// Вычисляем скорость расчёта модели, количество тиков в секунду.
			calculateTPS();

			// Выполняем расчёт модели.
			makeTick();

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

	// Если это остановка, обновим состояние до точного значения, чтобы после загрузки оно не отличалось.
	if (!is_active)
		InitResMaxArray();
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
	}
	catch (...) {
		// При ошибке загружаем чистую модель.
		globalWorld.loadModel(defaultFilename);
	}
}

// Вынесено из SaveModel() для удобства. Запись одного организма.
void World::doWriteOrganism(clan::IODevice &binFile, speciesDict_t& speciesDict, demi::Organism* organism)
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

	// Записываем собственные поля организма.
	organism->saveToFile(binFile);
}

demi::Organism* World::doReadOrganism(clan::IODevice &binFile, speciesDict_t& speciesDict, const clan::Point &center)
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
	return demi::Organism::createFromFile(binFile, center, species);
}

// Инициализирует массим максимумов на основе имеющихся количеств в точках, используется после загрузки.
void World::InitResMaxArray()
{
	// Очистим старые значения.
	memset(arResMax, 0, sizeof(unsigned long long) * elemCount);

	// Перебираем все точки и сохраняем количества.
	demi::Dot *cur = arDots;										// Первая точка массива.
	demi::Dot *last = cur + worldSize.width * worldSize.height;	// Точка после последней точки массива.
	while (cur < last) {

		// Обработка организма - при его распаде он вернёт вещества.
		demi::Organism* curOrganism = cur->organism;
		if (curOrganism) {

			// Если в исходной точке есть организм и его надо удалить, сделаем это.
			if (curOrganism->needDesintegration()) {
				// Из текущей точки организм удалит сам себя в деструкторе.
				delete curOrganism;
			}
		}

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

// Вычисляет вероятность мутации и возвращает истину, если она должна состояться.
bool World::activateMutation()
{
	// Если выпал ноль, значит шанс сработал.
	return rnd_Mutation(generator) == 0;
}


// Вычисляет скорость расчёта модели, количество тиков в секунду.
void World::calculateTPS()
{
	// Код по-аналогии с game_time.cpp
	uint64_t current_time = clan::System::get_microseconds();

	num_updates_in_2_seconds++;
	float delta_time_ms = (current_time - update_frame_start_time) / 1000.0f;

	if ((delta_time_ms < 0) || (delta_time_ms > 2000))		// Sample FPS every 2 seconds
	{
		if (delta_time_ms > 0)
		{
			current_tps = (int)((num_updates_in_2_seconds*1000.0f) / delta_time_ms);
		}
		num_updates_in_2_seconds = 0;
		update_frame_start_time = current_time;
	}
}

// Возвращает индекс элемента по названию. Когда элементов или реакций станет много, надо будет оптимизировать.
size_t World::findElemIndex(const std::string& elemName)
{
	// Перебираем все названия и возвращаем индекс.
	for (size_t i = 0; i != elemCount; ++i)
		if (arResNames[i] == elemName)
			return i;

	throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongReaction), elemName));
}



