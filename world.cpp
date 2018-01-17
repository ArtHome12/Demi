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


const float cGeothermRadius = 12.0f;

// Константы для чтения XML-файла модели
auto cResGlobalsWorldSize = "Globals/WorldSize";
auto cResGlobalsWorldSizeWidth = "width";
auto cResGlobalsWorldSizeHeightRatio = "heightRatio";

auto cResGlobalsAppearance = "Globals/Appearance";
auto cResGlobalsAppearanceTop = "top";
auto cResGlobalsAppearanceLeft = "left";
auto cResGlobalsAppearanceScale = "scale";

auto cResGlobalsLUCA = "Globals/LUCA";
auto cResGlobalsLUCAVisibility = "visibility";
auto cResGlobalsLUCAFissionBarier = "fissionBarrier";
auto cResGlobalsLUCAAliveColor = "aliveColor";
auto cResGlobalsLUCADeadColor = "deadColor";
auto cResGlobalsLUCAminActiveMetabolicRate = "minActiveMetabolicRate";
auto cResGlobalsLUCAminInactiveMetabolicRate = "minInactiveMetabolicRate";

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
auto cWrongBinVer = "WorldWrongBinaryFileVersion";
auto cWrongBinElemCount = "WorldWrongBinaryFileElementsCount";
auto cWrongBinElemName = "WorldWrongBinaryFileElementsName";
auto cWrongBinMarker = "WorldWrongBinaryFileMarker";
auto cWrongBinCannotReadDots = "WorldWrongBinaryFileCannotReadDots";
auto cWrongBinResmaxMarker = "WorldWrongBinaryFileResmaxMarker";
auto cWrongBinCannotReadResmax = "WorldWrongBinaryFileCannotReadResmax";


// Глобальный объект - неживой мир.
World globalWorld;

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
	// Сброс старой освещённости должен был быть сделан при перемешивании, но практически она затирается сама.

	// Определим систему координат с положением солнца в центре.
	LocalCoord coord(getPos(timeModel));

	// Двигаемся по всем точкам
	//
	float lightRadius = globalWorld.getLightRadius();
	for (float x = -lightRadius; x <= lightRadius; ++x)
		for (float y = -lightRadius; y <= lightRadius; ++y) {

			// Фактическое расстояние до центра координат.
			float r = sqrt(x*x + y*y);

			// Если оно больше заданного радиуса, ничего не делаем, иначе зададим освещённость, обратную расстоянию в долях от 0 до 1.
			coord.get_dot(x, y).setSolarEnergy(r < lightRadius ? (lightRadius - r) / lightRadius : 0.0f);
		}

}


clan::Pointf Solar::getPos(const DemiTime &timeModel)
{
	// Возвращает позицию для солнца в зависимости от времени.

	// Размеры мира.
	const clan::Sizef worldSize = globalWorld.get_worldSize();

	// Позиция по горизонтали зависит от времени суток.
	// Солнце двигается с востока на запад пропорционально прошедшей доле суток.
	clan::Pointf result(worldSize.width * (1.0f - float(timeModel.sec - 1) / (cTicksInDay - 1)), 0.0f);

	// Позиция по вертикали зависит от дня года.
	// Солнце полгода двигается от одной границы тропиков до другой и полгода в обратном направлении, 
	// то есть через полгода позиция повторяется.
	
	// Половина года, для удобства.
	const float halfYear = cDaysInYear / 2;

	// Высота тропиков
	const float tropic = globalWorld.getTropicHeight();

	// Когда идёт первая половина года, надо от экватора отнимать долю, а когда вторая - прибавлять.
	//
	if (timeModel.day < halfYear)
		result.y = (worldSize.height - tropic) / 2 + timeModel.day * tropic / halfYear;
	else
		result.y = (worldSize.height + tropic) / 2 - (timeModel.day - halfYear) * tropic / halfYear;

	return result;
}



// =============================================================================
// Земная поверхность, двумерный массив точек.
// =============================================================================
World::World() : generator(random_device()), rnd_angle(0, 7), rnd_Coord(0, 12 - 1)

{
	// Создадим поток. Он не стартанёт до установки флага.
	thread = std::thread(&World::workerThread, this);
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
	delete[] arDotsCopy;
	delete[] arResColors;
	delete[] arResMax;
	delete[] arResNames;
	delete[] arResVisible;
	delete[] arResVolatility;
	delete[] arEnergy;
}


void World::MakeTick()
{
	// Обновить состояние.
	//
	// Обновляем время мира.
	timeModel.MakeTick();

	// Облучаем мир солнечной энергией.
	solar.Shine(timeModel);

	// Перемешивание ресурсов из-за диффузии.
	Diffusion();

	// Передаём управление живым организмам.
	//

	// Надо оценить эффективность данного способа, мож через случайное число будет быстрее.
	std::shuffle(animals.begin(), animals.end(), generator);

	// Используем традиционый цикл, так как иначе итераторы портятся после изменения вектора.
	int cnt = animals.size();
	for (int i = 0; i < cnt; ++i) {

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
}



void World::FillRectResource(int resId, float amount, const clan::Rectf &rect)
{
	// Задаёт распределение ресурсов по указанной прямоугольной области в указанном количестве.
	//
	// Откорректируем максимум, если требуется.
	//
	if (arResMax[resId] < amount)
		arResMax[resId] = amount;

	for (float x = rect.left; x < rect.right; ++x)
		for (float y = rect.top; y < rect.bottom; ++y)
			arDots[int(x + y * worldSize.width)].res[resId + 2] = amount;
}


void World::Diffusion()
{
	// Диффузия ресурсов.

	// Границы массива, за которую заходить нельзя.
	Dot *cur = arDots;											// Исходная точка для переноса ресурсов - первая точка массива.
	Dot *last = cur + int(worldSize.width * worldSize.height);	// Точка после последней точки массива.
	Dot *dest;													// Конечная точка

	// Направление движения
	std::uniform_int_distribution<> rnd_angle(0, 7);
	// Элемент
	std::uniform_int_distribution<> rnd_Elem(0, elemCount-1);
	// Приращение координаты
	std::uniform_int_distribution<> rnd_Coord(0, 12-1);

	while (true) {

		int rnd = rnd_angle(generator);			// от 0 до 7, направление движения
		int rndResA = rnd_Elem(generator);		// случайный элемент.
		int rndResB = rndResA + 2;				// плюс пропускаем ячейки под солнечную и геотермальную энергии.

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
			dest = cur + 1 + int(worldSize.width);
			break;
		case 2:	// на юг.
			dest = cur + int(worldSize.width);
			break;
		case 3:	// на юго-запад.
			dest = cur - 1 + int(worldSize.width);
			break;
		case 4:	// на запад.
			dest = cur - 1;
			break;
		case 5:	// на северо-запад.
			dest = cur - 1 - int(worldSize.width);
			break;
		case 6:	// на север.
			dest = cur - int(worldSize.width);
			break;
		default:	// на северо-восток.
			dest = cur + 1 - int(worldSize.width);
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
		const float amount = fromDot.res[rndResB] * arResVolatility[rndResA];
		fromDot.res[rndResB] -= amount;
		toDot.res[rndResB] += amount;

		// Обновим максимумы.
		//
		if (arResMax[rndResA] < toDot.res[rndResB])
			arResMax[rndResA] = toDot.res[rndResB];

		// Если в исходной точке есть организм, а в конечной его нет, то попытаемся перенести и его.
		if (fromDot.organism != nullptr && toDot.organism == nullptr && fromDot.organism->canMove()) {
			// Координаты новой точки.
			int index = dest - arDots;
			float y = truncf(index / worldSize.width);
			float x = index - y * worldSize.width;

			fromDot.organism->moveTo(clan::Pointf(x, y));
		}
	}
}


void World::AddGeothermal(int i, const clan::Pointf &coord)
{
	// Задаёт местоположение источников геотермальной энергии.
	//

	arEnergy[i].coord = coord;

	// Так как геотермальные источники на данный момент считаем неизменяемыми, сразу же зададим распределение энергии.
	//

	// Определим систему координат.
	LocalCoord geothermalCoord(coord);

	for (float xp = -cGeothermRadius; xp <= cGeothermRadius; ++xp)
		for (float yp = -cGeothermRadius; yp <= cGeothermRadius; ++yp) {

			// Фактическое расстояние до центра координат.
			float r = sqrt(xp*xp + yp*yp);

			// Если оно больше заданного радиуса, ничего не делаем, иначе зададим освещённость, обратную расстоянию в долях от 0 до 1.
			if (r < cGeothermRadius) 
				geothermalCoord.get_dot(xp, yp).setGeothermalEnergy((cGeothermRadius - r) / cGeothermRadius);
		}
}


void World::LoadModel(const std::string &filename)
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

	// Инициализируем размеры мира.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// Ширина мира.
	worldSize.width = float(prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000));

	// Высота мира
	worldSize.height = round(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f));
	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongSize), int(worldSize.width), int(worldSize.height)));

	// Радиус солнечного пятна и высота тропиков зависит от размера мира.
	lightRadius = round(0.9f * worldSize.height / 2);
	tropicHeight = round(worldSize.height / 5);

	// Считываем протоорганизм. Всегда создаём один протоорганизм, считаем что образование живого мира из неживого не останавливается.
	// Создаём вид протоорганизма по указанным координатам. Внимание - ниже он может быть переопределён из двоичного файла.
	prop = resDoc->get_resource(cResGlobalsLUCA).get_element();
	species = std::make_shared<demi::Species>();
	species->name = "LUCA";
	species->author = "Demi";
	species->visible = prop.get_attribute_bool(cResGlobalsLUCAVisibility);
	species->cells.push_back(std::make_shared<demi::CellAbdomen>());
	species->fissionBarrier = prop.get_attribute_float(cResGlobalsLUCAFissionBarier);
	species->aliveColor = clan::Colorf(prop.get_attribute(cResGlobalsLUCAAliveColor));
	species->deadColor = clan::Colorf(prop.get_attribute(cResGlobalsLUCADeadColor));

	const clan::Pointf LUCAPos(float(prop.get_attribute_int("x")), float(prop.get_attribute_int("y")));
	const std::string LUCAReactionName = prop.get_attribute("reaction");
	demi::Organism::minActiveMetabolicRate = prop.get_attribute_float(cResGlobalsLUCAminActiveMetabolicRate);
	demi::Organism::minInactiveMetabolicRate = prop.get_attribute_float(cResGlobalsLUCAminInactiveMetabolicRate);


	// Инициализируем внешний вид проекта.
	prop = resDoc->get_resource(cResGlobalsAppearance).get_element();
	appearanceTopLeft.x = float(prop.get_attribute_int(cResGlobalsAppearanceLeft, int(appearanceTopLeft.x)));
	appearanceTopLeft.y = float(prop.get_attribute_int(cResGlobalsAppearanceTop, int(appearanceTopLeft.y)));
	appearanceScale = prop.get_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// Прочитаем время.
	prop = resDoc->get_resource(cResGlobalsTime).get_element();
	timeModel.year = prop.get_attribute_int(cResGlobalsTimeYear, 1);
	timeModel.day = prop.get_attribute_int(cResGlobalsTimeDay, 1);
	timeModel.sec = prop.get_attribute_int(cResGlobalsTimeSecond, 0);
	timeBackup = timeModel;

	// Названия ресурсов.
	const std::vector<std::string>& names = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);

	// Количество ресурсов.
	elemCount = int(names.size());

	// Источники энергии.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);

	// Количество источников.
	energyCount = int(energy.size());

	// Выделим память под массивы.
	arDots = new Dot[int(worldSize.width * worldSize.height)];	// точки поверхности.
	arResColors = new clan::Colorf[elemCount];					// цвета элементов.
	arResMax = new float[elemCount];							// максимальные концентрации элементов в одной точке.
	arResNames = new std::string[elemCount];					// названия элементов.
	arResVisible = new bool[elemCount];							// Видимость элементов.
	arResVolatility = new float[elemCount];						// летучесть элементов.
	arEnergy = new Geothermal[energyCount];						// геотермальные источники.

	// Инициализируем массив максимумов единицами, во-избежание деления на ноль.
	for (int i = 0; i < elemCount; ++i)
		arResMax[i] = 1.0f;

	// Считываем названия элементов.
	for (int i = 0; i < elemCount; ++i) {

		// Элемент.
		clan::XMLResourceNode &res = resDoc->get_resource(names[i]);

		// Название элемента.
		arResNames[i] = res.get_name();

		// Свойства элемента.
		clan::DomElement &prop = res.get_element();

		// Цвет для отображения.
		arResColors[i] = clan::Colorf(prop.get_attribute(cResElementsColor));

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

			float r = float(rectItem.get_attribute_int("right"));
			if (r == 0)
				r = worldSize.width;

			float b = float(rectItem.get_attribute_int("bottom"));
			if (b == 0)
				b = worldSize.height;

			// Область.
			clan::Rectf rect(float(rectItem.get_attribute_int("left")), float(rectItem.get_attribute_int("top")), r, b);

			// Заполняем точки.
			FillRectResource(i, rectItem.get_attribute_float("amount"), rect);
		}
	}

	// Считываем местоположение геотермальных источников.
	for (int i = 0; i < energyCount; ++i) {
		clan::DomElement &prop = resDoc->get_resource(energy[i]).get_element();
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaPoint);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// Найденный элемент с информацией о распределении ресурса.
			clan::DomElement &pointItem = nodes.item(j).to_element();

			// Заполняем точки.
			AddGeothermal(i, clan::Pointf(float(pointItem.get_attribute_int("x")), float(pointItem.get_attribute_int("y"))));
		}
	}

	// Считываем химические реакции.
	// Названия реакций.
	reactions.clear();
	const std::vector<std::string>& reactionsNames = resDoc->get_resource_names_of_type(cResReactionsType, cResReactionsSection);
	for (auto & reactionName : reactionsNames) {
		// Создаём реакцию.
		auto curReaction = std::make_shared<demi::ChemReaction>();

		// Количество необходимой энергии и выхлоп жизненной энергии.
		clan::XMLResourceNode &res = resDoc->get_resource(reactionName);
		clan::DomElement &prop = res.get_element();
		curReaction->name = res.get_name();
		curReaction->geoEnergy = prop.get_attribute_float(cResReactionsGeoEnergy);
		curReaction->solarEnergy = prop.get_attribute_float(cResReactionsSolarEnergy);
		curReaction->vitalityProductivity = prop.get_attribute_float(cResReactionsVitalityProductivity);

		// Реагенты слева.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResReactionsLeftReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// По названию получаем индекс элемента и добавляем его в реакцию вместе с количеством.
			auto pos = std::distance(names.begin(), find(names.begin(), names.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->leftReagents.push_back(reagent);
		}

		// Реагенты справа.
		nodes = prop.get_elements_by_tag_name(cResReactionsRightReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// По названию получаем индекс элемента и добавляем его в реакцию вместе с количеством.
			auto pos = std::distance(names.begin(), find(names.begin(), names.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->rightReagents.push_back(reagent);
		}

		// Сохраняем реакцию в списке реакций.
		reactions.insert(std::pair<std::string, std::shared_ptr<demi::ChemReaction>>(curReaction->name, curReaction));
	}

	// Доинициализируем протоорганизм ссылкой на реакцию.
	species->reaction = reactions[LUCAReactionName];


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
		auto &strElemCountAwait = "ElementsCount:" + clan::StringHelp::int_to_text(elemCount);
		if (strElemCount != strElemCountAwait)
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemCount), strElemCountAwait, strElemCount));

		// Названия элементов.
		for (int i = 0; i < elemCount; ++i) {
			auto &elemName = binFile.read_string_nul();
			if (elemName != arResNames[i])
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemName), arResNames[i], elemName));
		}

		// Маркер начала списка видов организмов.
		auto &strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Species:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Species:", strSecMarker));

		// Считываем виды организмов рекурсивно.
		species = DoReadSpecies(binFile, nullptr);

		// Маркер начала массива точек.
		strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Dots:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Dots:", strSecMarker));

		// Считываем точки неживого мира. Клетки будут размещены при считывании организмов.
		int dotsCount = int(worldSize.width * worldSize.height);
		int dotSize = Dot::getSizeInMemory();
		for (int i = 0; i < dotsCount; ++i) {
			if (binFile.read(arDots[i].res, dotSize) != dotSize)
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadDots), i));
		}

		// Маркер начала массива концентраций.
		auto &strResMaxMarker = binFile.read_string_nul();
		if (strResMaxMarker != "ResMax:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinResmaxMarker), strResMaxMarker));

		// Считываем максимальные концентрации.
		dotSize = elemCount * sizeof(float);
		if (binFile.read(arResMax, dotSize) != dotSize)
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadResmax)));

		// Закроем файл.
		binFile.close();
	}

	// Создаём экземпляр протоорганизма и размещаем его в геотермальном источнике.
	animals.clear();
	animals.push_back(new demi::Organism(species, LUCAPos, 0, species->fissionBarrier, species->fissionBarrier));
}

// Рекурсивная функция для считывания видов организмов.
std::shared_ptr<demi::Species> World::DoReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor)
{
	// Считываем текущий вид.

	// Создаём вид и инициализиуем основные поля.
	auto retVal = std::make_shared<demi::Species>();
	retVal->ancestor = ancestor;
	retVal->name = binFile.read_string_nul();
	retVal->author = binFile.read_string_nul();
	retVal->visible = binFile.read_int8() != 0;

	// Метаболитическая реакция.
	std::string reactionName = binFile.read_string_nul();
	retVal->reaction = reactions[reactionName];

	// Создаём и считываем клетки.

	// Количество клеток.
	int64_t cnt = binFile.read_int64();
	
	// В цикле создаём все клетки.
	for (int i = 0; i < cnt; ++i) {

		// Тип клетки.
		int64_t cellType = binFile.read_int64();

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
	cnt = binFile.read_int64();

	// Считываем и сохраняем потомков рекурсивно.
	for (int i = 0; i < cnt; ++i)
		retVal->descendants.push_back(DoReadSpecies(binFile, retVal));
	
	// Возвращаем считанный элемент.
	return retVal;
}

// Рекурсивная функция для записи видов организмов.
void World::DoWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies)
{
	// Записываем основные поля.
	binFile.write_string_nul(aSpecies->name);
	binFile.write_string_nul(aSpecies->author);
	binFile.write_int8(aSpecies->get_visible());
	binFile.write_string_nul(aSpecies->reaction->name);

	// Записываем клетки.
	binFile.write_int64(aSpecies->cells.size());
	for (auto& cell : aSpecies->cells) {
		// Тип клетки.
		int64_t cellType = cell->getCellType();
		binFile.write_int64(cellType);
	}

	// Записываем дочерние виды.

	// Количество дочерних видов.
	binFile.write_int64(aSpecies->descendants.size());

	// Сами виды рекурсивно.
	for (auto &spec : aSpecies->descendants)
		DoWriteSpecies(binFile, spec);
}


void World::SaveModel(const std::string &filename)
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
	prop.set_attribute_int(cResGlobalsAppearanceLeft, int(appearanceTopLeft.x));
	prop.set_attribute_int(cResGlobalsAppearanceTop, int(appearanceTopLeft.y));
	prop.set_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// Запишем видимость протоорганизма.
	prop = pResDoc->get_resource(cResGlobalsLUCA).get_element();
	prop.set_attribute_bool(cResGlobalsLUCAVisibility, species->visible);

	// Записываем время.
	prop = pResDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, timeModel.year);
	prop.set_attribute_int(cResGlobalsTimeDay, timeModel.day);
	prop.set_attribute_int(cResGlobalsTimeSecond, timeModel.sec);

	// Запишем видимость элементов.
	for (int i = 0; i < elemCount; ++i) {

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
	binFile.write_string_nul("ElementsCount:" + clan::StringHelp::int_to_text(elemCount));

	// Запишем названия элементов.
	for (int i = 0; i < elemCount; ++i)
		binFile.write_string_nul(arResNames[i]);

	// Записываем маркер организмов.
	binFile.write_string_nul("Species:");

	// Записываем виды организмов рекурсивно.
	DoWriteSpecies(binFile, species);

	// Записываем маркер начала массива точек.
	binFile.write_string_nul("Dots:");

	// Записываем точки. Сразу все не получается, так как массив не плоский.
	int dotsCount = int(worldSize.width * worldSize.height);
	int dotSize = Dot::getSizeInMemory();
	for (int i = 0; i < dotsCount; ++i)
		binFile.write(arDots[i].res, dotSize);

	// Записываем маркер начала массива максимумов.
	binFile.write_string_nul("ResMax:");

	// Записываем максимумы концентраций ресурсов.
	binFile.write(arResMax, elemCount * sizeof(float));

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
			MakeTick();

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

void World::RunEvolution(bool is_active)
{
	// Приостанавливает или продолжает расчёт модели
	std::unique_lock<std::mutex> lock(threadMutex);
	threadRunFlag = is_active;
	threadEvent.notify_all();
}

// Начинает расчёт заново.
void World::ResetModel(const std::string &modelFilename, const std::string &defaultFilename)
{
	// Удаляем двоичный файл.
	if (clan::FileHelp::file_exists(modelFilename + "b"))
		clan::FileHelp::delete_file(modelFilename + "b");

	// Попытаемся загрузить модель.
	try {
		globalWorld.LoadModel(modelFilename);
		timeModel = DemiTime();
		timeBackup = timeModel;
	}
	catch (...) {
		// При ошибке загружаем чистую модель.
		globalWorld.LoadModel(defaultFilename);
	}
}

//Dot * World::getCopyDotsArray()
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

