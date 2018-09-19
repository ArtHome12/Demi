/* ===============================================================================
	������������� �������� ������ ����.
	������ ��� �������� ����������� - �����, ������������.
	24 august 2013.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "world.h"


const int cGeothermRadius = 12;

// ��������� ��� ������ XML-����� ������
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
auto cResGlobalsLUCADesintegrationVitalityBarrier = "desintegrationVitalityBarrier";

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

auto cResAreaRect = "rect";	// ��� ��� ������������� ������� ���������� ������������� �������/����������.
auto cResAreaPoint = "point";	// ��� ��� �����.

auto cSolar = "Solar";			// ����������� ��������� �������.
auto cEnergy = "Geothermal";	// ����������� ������������� �������.

// ��������� �������
auto cWrongSize = "WorldWrongSize";
auto cWrongBinVer = "WorldWrongBinaryFileVersion";
auto cWrongBinElemCount = "WorldWrongBinaryFileElementsCount";
auto cWrongBinElemName = "WorldWrongBinaryFileElementsName";
auto cWrongBinMarker = "WorldWrongBinaryFileMarker";
auto cWrongBinCannotReadDots = "WorldWrongBinaryFileCannotReadDots";
auto cWrongBinResmaxMarker = "WorldWrongBinaryFileResmaxMarker";
auto cWrongBinCannotReadResmax = "WorldWrongBinaryFileCannotReadResmax";


// ���������� ������ - ������� ���.
World globalWorld;

// =============================================================================
// ������ �������� �������
// =============================================================================
void DemiTime::MakeTick()
{
	// ���������� ���� ���.
	//
	// ���������� ���� ������� � ���������, �� �������� �� ������ �����
	if (++sec >= cTicksInDay) {

		// �������� �������, ���������� ���� ���� � ���������� ��������.
		sec = 0;
		if (++day >= cDaysInYear) {
			day = 1;

			// ���� ������ ���������� �� ��������� �����.
			year++;
		}
	}
}

std::string DemiTime::getDateStr() const
{
	// ���������� ������ � �������� ������.
	//
	return clan::string_format("%1:%2:%3", year, day, sec);
}


// =============================================================================
// �������� ��������� �������
// =============================================================================
void Solar::Shine(const DemiTime &timeModel)
{
	// �������� �������� ��� ���������� ������� ������� �����.
	//
	// ����� ������ ������������ ������ ��� ���� ������ ��� �������������, �� ����������� ��� ���������� ����.

	// ��������� ������� ��������� � ���������� ������ � ������.
	LocalCoord coord(getPos(timeModel));

	int lightRadius = globalWorld.getLightRadius();
	for (int x = -lightRadius; x <= lightRadius; ++x)
		for (int y = -lightRadius; y <= lightRadius; ++y) {

			// ����������� ���������� �� ������ ���������.
			double r = sqrt(x*x + y*y);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			r = r < lightRadius ? float((lightRadius - r) / lightRadius) : 0.0f;
			
			coord.get_dot(x, y).setSolarEnergy(float(r));
		}
}


clan::Point Solar::getPos(const DemiTime &timeModel)
{
	// ���������� ������� ��� ������ � ����������� �� �������.

	// ������� ����.
	const clan::Size &worldSize = globalWorld.get_worldSize();

	// ������� �� ����������� ������� �� ������� �����.
	// ������ ��������� � ������� �� ����� ��������������� ��������� ���� �����.
	 int x = int(worldSize.width * (1.0f - float(timeModel.sec) / (cTicksInDay - 1.0f)) + 0.5f);

	// ������� �� ��������� ������� �� ��� ����.
	// ������ ������� ��������� �� ����� ������� �������� �� ������ � ������� � �������� �����������, 
	// �� ���� ����� ������� ������� �����������.
	
	// �������� ����, ��� ��������.
	const int halfYear = int(cDaysInYear / 2 + 0.5f);

	// ������ ��������
	const int& tropic = globalWorld.getTropicHeight();

	// ����� ��� ������ �������� ����, ���� �� �������� �������� ����, � ����� ������ - ����������.
	int y = int((timeModel.day < halfYear ? (worldSize.height - tropic) / 2.0f + timeModel.day * tropic / halfYear : (worldSize.height + tropic) / 2.0 - (timeModel.day - halfYear) * tropic / halfYear) +0.5);

	return clan::Point(x, y);
}



// =============================================================================
// ������ �����������, ��������� ������ �����.
// =============================================================================
World::World() : generator(random_device()), rnd_angle(0, 7), rnd_Coord(0, 12 - 1)

{
	// �������� �����. �� �� �������� �� ��������� �����.
	thread = std::thread(&World::workerThread, this);
}



World::~World()
{
	// �������� ������ ������, ���� �� ���� ������, �� ��� �� ����� ����������� ���������.
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
	// �������� ���������.
	//
	// ��������� ����� ����.
	timeModel.MakeTick();

	// �������� ��� ��������� ��������.
	solar.Shine(timeModel);

	// ������������� �������� ��-�� ��������.
	diffusion();

	// ������� ���������� ����� ����������.
	//
	
	// ���� ������� ������������� ������� �������, ��� ����� ��������� ����� ����� �������.
	std::shuffle(animals.begin(), animals.end(), generator);

	// ���������� ����������� ����, ��� ��� ����� ��������� �������� ����� ��������� �������.
	int cnt = animals.size();
	for (int i = 0; i < cnt; ++i) {

		demi::Organism &animal = *animals[i];

		// ��������, ���� �������� �����, ��� ���� ������� �� ������.
		if (!animal.isAlive()) {
			// ������ ��� � ��������� � ������� ���������.
			animals[i] = animals.back();
			animals.pop_back();
			--cnt;
			continue;
		}

		// ������� ��� ����������. ���� ������� �������� �� 0, ���� ��������� ����� �������� � ������.
		demi::Organism * newBorn = animal.makeTickAndGetNewBorn();
		if (newBorn != nullptr) {
			animals.push_back(newBorn);
			++cnt;
		}
	}

	// ������ ��������� ��������������, ���� ���� �����.
	Dot& protoDot = LocalCoord(LUCAPos).get_dot(0, 0);
	if (protoDot.organism == nullptr)
		animals.push_back(new demi::Organism(species, LUCAPos, 0, species->fissionBarrier, species->fissionBarrier));
}



void World::fillRectResource(int resId, unsigned long long amount, const clan::Rect &rect)
{
	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	//
	// �������������� ��������, ���� ���������.
	//
	if (arResMax[resId] < amount)
		arResMax[resId] = amount;

	for (int x = rect.left; x < rect.right; ++x)
		for (int y = rect.top; y < rect.bottom; ++y)
			arDots[getDotIndexFromXY(x, y)].setElementAmount(resId, amount);
}


// �������� ��������.
void World::diffusion()
{
	// �������� ������� arResMax, ������������ ��� ������ ����� ����� �� ������ ������������ ������������������ �������� � ���, �� ������ ������ ������ �����.
	// � �����-�� �������������� ���� �� ���������������.���� ������ ��� ��� ����������, ���� ��� ��������, ���� ����� �� �������, �� �������� ���������� ������ ��������������� ��������� �������� ���� ��� ����������������
	// ��������� ������.���� ������ ����� ����������� ��������� ����� �������, � ����� ����� ��������� �����, ��� ����� ���������� ��� ���. ������� ������ ��� ������ ���� ������������ ������������ ������� �� �������.
	for (int i = 0; i < elemCount; ++i)
		if (arResMax[i])
			--arResMax[i];


	// ������� �������, �� ������� �������� ������.
	Dot *cur = arDots;											// �������� ����� ��� �������� �������� - ������ ����� �������.
	Dot *last = cur + worldSize.width * worldSize.height;		// ����� ����� ��������� ����� �������.
	Dot *dest;													// �������� �����

	// ����������� ��������
	std::uniform_int_distribution<> rnd_angle(0, 7);
	// �������
	std::uniform_int_distribution<> rnd_Elem(0, elemCount-1);
	// ���������� ����������
	std::uniform_int_distribution<> rnd_Coord(0, 12-1);

	while (true) {

		int rnd = rnd_angle(generator);			// �� 0 �� 7, ����������� ��������
		const int rndResIndex = rnd_Elem(generator);		// ��������� �������.

		// ���������� �������� ���������� ��������� ��������� ������.
		cur += rnd_Coord(generator);

		// ���� ���������� ��� �����, �����������.
		if (cur >= last)
			break;

		// ���������� �������� ���������� - � ���� �� ������ �� ��������.
		switch (rnd) {
		case 0:	// �� ������.
			dest = cur + 1;
			break;
		case 1: // �� ���-������.
			dest = cur + 1 + worldSize.width;
			break;
		case 2:	// �� ��.
			dest = cur + worldSize.width;
			break;
		case 3:	// �� ���-�����.
			dest = cur - 1 + worldSize.width;
			break;
		case 4:	// �� �����.
			dest = cur - 1;
			break;
		case 5:	// �� ������-�����.
			dest = cur - 1 - worldSize.width;
			break;
		case 6:	// �� �����.
			dest = cur - worldSize.width;
			break;
		default:	// �� ������-������.
			dest = cur + 1 - worldSize.width;
		}
		
		// �������������� � ������ ������ �� �������, ����� �� �������� �������� � ������� ���� ��-�� ������
		// � ���� ����, ��� ����������� ����� �������� ����� ��������� � ��������.
		if (dest < arDots)
			dest = last - (arDots - dest);
		else if (dest >= last)
			dest = arDots + (dest - last);

		// ���������� ������� �������� �� �������� ����� � �������� �������� ��������� �������.
		Dot &fromDot = *cur;
		Dot &toDot = *dest;
		unsigned long long amount = unsigned long long(fromDot.res[rndResIndex] * arResVolatility[rndResIndex] + 0.5f);

		// � ��������, ����� �������� � ����� ���� � ������������ �� ��������� ��� ����, ���������� 1 �������, ����� ��� ��������� ����������� ������� � ���������� ����� ����������.
		if (!amount && fromDot.res[rndResIndex] > 0)
			amount = 1;

		fromDot.res[rndResIndex] -= amount;
		toDot.res[rndResIndex] += amount;

		// ������� ���������.
		//
		if (arResMax[rndResIndex] < toDot.res[rndResIndex])
			arResMax[rndResIndex] = toDot.res[rndResIndex];

		// ��������� ���������.
		demi::Organism* curOrganism = fromDot.organism;
		if (curOrganism == nullptr)
			continue;

		// ���� � �������� ����� ���� �������� � ��� ���� �������, ������� ���.
		if (curOrganism->needDesintegration()) {
			// �� ������� ����� �������� ������ ��� ���� � �����������.
			delete curOrganism;
		}
		else {
			// ���� � �������� ����� ��� ���������, �� ���������� ��������� ��������.
			if (toDot.organism == nullptr && curOrganism->canMove())
				// ���������� ����� �����.
				curOrganism->moveTo(getDotXYFromIndex(dest - arDots));

			// ��������� ��������� �������, ����������� �� �������.
			curOrganism->processInactiveVitality();
		}
	}
}

// ���������� ���������� ����� �� ���������� �������.
clan::Point World::getDotXYFromIndex(int index)
{
	int y = int(index / worldSize.width);
	int x = index - y * worldSize.width;
	return clan::Point(x, y);
}


void World::addGeothermal(int i, const clan::Point &coord)
{
	// ����� �������������� ���������� ������������� �������.
	//

	arEnergy[i].coord = coord;

	// ��� ��� ������������� ��������� �� ������ ������ ������� �������������, ����� �� ������� ������������� �������.
	//

	// ��������� ������� ���������.
	LocalCoord geothermalCoord(coord);

	for (int xp = -cGeothermRadius; xp <= cGeothermRadius; ++xp)
		for (int yp = -cGeothermRadius; yp <= cGeothermRadius; ++yp) {

			// ����������� ���������� �� ������ ���������.
			double r = sqrt(xp*xp + yp*yp);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			if (r < cGeothermRadius) 
				geothermalCoord.get_dot(xp, yp).setGeothermalEnergy(float((cGeothermRadius - r) / cGeothermRadius));
		}
}


void World::loadModel(const std::string &filename)
{
	// ��������� ������ �� xml-�����

	// ���� ����� ��� �������, ������� �� ������.
	{
		std::unique_lock<std::mutex> lock(threadMutex);
		if (threadRunFlag)
			throw clan::Exception(pSettings->LocaleStr("WorldLoadModelErrorNeedToStop"));
	}

	// ������� XML ����.
	auto resDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// �������������� ������� ����.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// ������ ����.
	worldSize.width = prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000);

	// ������ ����
	worldSize.height = int(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f) + 0.5f);
	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongSize), worldSize.width, worldSize.height));

	// ������ ���������� ����� � ������ �������� ������� �� ������� ����.
	lightRadius = int(0.9f * worldSize.height / 2 + 0.5f);
	tropicHeight = int(0.2f * worldSize.height + 0.5f);

	// ��������� �������������. ������ ������ ���� �������������, ������� ��� ����������� ������ ���� �� �������� �� ���������������.
	// ������ ��� �������������� �� ��������� �����������. �������� - ���� �� ����� ���� ������������ �� ��������� �����.
	prop = resDoc->get_resource(cResGlobalsLUCA).get_element();
	species = std::make_shared<demi::Species>();
	species->name = "LUCA";
	species->author = "Demi";
	species->visible = prop.get_attribute_bool(cResGlobalsLUCAVisibility);
	species->cells.push_back(std::make_shared<demi::CellAbdomen>());
	species->fissionBarrier = prop.get_attribute_int(cResGlobalsLUCAFissionBarier);
	species->aliveColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCAAliveColor)));
	species->deadColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCADeadColor)));

	LUCAPos = clan::Point(prop.get_attribute_int("x"), prop.get_attribute_int("y"));
	const std::string LUCAReactionName = prop.get_attribute("reaction");
	demi::Organism::minActiveMetabolicRate = prop.get_attribute_int(cResGlobalsLUCAminActiveMetabolicRate);
	demi::Organism::minInactiveMetabolicRate = prop.get_attribute_int(cResGlobalsLUCAminInactiveMetabolicRate);
	demi::Organism::desintegrationVitalityBarrier = prop.get_attribute_int(cResGlobalsLUCADesintegrationVitalityBarrier);

	// �������������� ������� ��� �������.
	prop = resDoc->get_resource(cResGlobalsAppearance).get_element();
	appearanceTopLeft.x = prop.get_attribute_int(cResGlobalsAppearanceLeft, appearanceTopLeft.x);
	appearanceTopLeft.y = prop.get_attribute_int(cResGlobalsAppearanceTop, appearanceTopLeft.y);
	appearanceScale = prop.get_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// ��������� �����.
	prop = resDoc->get_resource(cResGlobalsTime).get_element();
	timeModel.year = prop.get_attribute_int(cResGlobalsTimeYear, 1);
	timeModel.day = prop.get_attribute_int(cResGlobalsTimeDay, 1);
	timeModel.sec = prop.get_attribute_int(cResGlobalsTimeSecond, 0);
	timeBackup = timeModel;

	// �������� ��������.
	const std::vector<std::string>& names = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);

	// ���������� ��������.
	elemCount = int(names.size());

	// ��������� �������.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);

	// ���������� ����������.
	energyCount = int(energy.size());

	// ������� ������ ��� �������.
	arDots = new Dot[worldSize.width * worldSize.height];	// ����� �����������.
	arResColors = new clan::Color[elemCount];				// ����� ���������.
	arResMax = new unsigned long long [elemCount];			// ������������ ������������ ��������� � ����� �����.
	arResNames = new std::string[elemCount];				// �������� ���������.
	arResVisible = new bool[elemCount];						// ��������� ���������.
	arResVolatility = new float[elemCount];					// ��������� ���������.
	arEnergy = new Geothermal[energyCount];					// ������������� ���������.

	// �������������� ������ ���������� ���������, ��-��������� ������� �� ����.
	for (int i = 0; i < elemCount; ++i)
		arResMax[i] = 1;

	// ��������� �������� ���������.
	for (int i = 0; i < elemCount; ++i) {

		// �������.
		clan::XMLResourceNode &res = resDoc->get_resource(names[i]);

		// �������� ��������.
		arResNames[i] = res.get_name();

		// �������� ��������.
		clan::DomElement &prop = res.get_element();

		// ���� ��� �����������. � ������ Color ��� ������-�� ������ �� �����, ���������� ����� ��������������.
		arResColors[i] = clan::Color(clan::Colorf(prop.get_attribute(cResElementsColor)));

		// ���������, �������� �� 0 �� 1.
		arResVolatility[i] = prop.get_attribute_float(cResElementsVolatility);

		// ��������� ��������.
		arResVisible[i] = prop.get_attribute_bool(cResElementsVisibility, true);

		// ������� ��� ���������� ��������.

		// ���� ������������� ������� � ����� ������������� �������.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaRect);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// ��������� ������� � ����������� � ������������� �������.
			clan::DomElement &rectItem = nodes.item(j).to_element();

			int r = rectItem.get_attribute_int("right");
			if (!r)
				r = worldSize.width;

			int b = rectItem.get_attribute_int("bottom");
			if (!b)
				b = worldSize.height;

			// �������.
			clan::Rect rect(rectItem.get_attribute_int("left"), rectItem.get_attribute_int("top"), r, b);

			// ��������� �����.
			fillRectResource(i, rectItem.get_attribute_int("amount"), rect);
		}
	}

	// ��������� �������������� ������������� ����������.
	for (int i = 0; i < energyCount; ++i) {
		clan::DomElement &prop = resDoc->get_resource(energy[i]).get_element();
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaPoint);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// ��������� ������� � ����������� � ������������� �������.
			clan::DomElement &pointItem = nodes.item(j).to_element();

			// ��������� �����.
			addGeothermal(i, clan::Point(pointItem.get_attribute_int("x"), pointItem.get_attribute_int("y")));
		}
	}

	// ��������� ���������� �������.
	// �������� �������.
	reactions.clear();
	const std::vector<std::string>& reactionsNames = resDoc->get_resource_names_of_type(cResReactionsType, cResReactionsSection);
	for (auto & reactionName : reactionsNames) {
		// ������ �������.
		auto curReaction = std::make_shared<demi::ChemReaction>();

		// ���������� ����������� ������� � ������ ��������� �������.
		clan::XMLResourceNode &res = resDoc->get_resource(reactionName);
		clan::DomElement &prop = res.get_element();
		curReaction->name = res.get_name();
		curReaction->geoEnergy = prop.get_attribute_float(cResReactionsGeoEnergy);
		curReaction->solarEnergy = prop.get_attribute_float(cResReactionsSolarEnergy);
		curReaction->vitalityProductivity = prop.get_attribute_int(cResReactionsVitalityProductivity);

		// �������� �����.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResReactionsLeftReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// �� �������� �������� ������ �������� � ��������� ��� � ������� ������ � �����������.
			auto pos = std::distance(names.begin(), find(names.begin(), names.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->leftReagents.push_back(reagent);
		}

		// �������� ������.
		nodes = prop.get_elements_by_tag_name(cResReactionsRightReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// �� �������� �������� ������ �������� � ��������� ��� � ������� ������ � �����������.
			auto pos = std::distance(names.begin(), find(names.begin(), names.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->rightReagents.push_back(reagent);
		}

		// ��������� ������� � ������ �������.
		reactions.insert(std::pair<std::string, std::shared_ptr<demi::ChemReaction>>(curReaction->name, curReaction));
	}

	// ���������������� ������������� ������� �� �������.
	species->reaction = reactions[LUCAReactionName];

	// ����� �������� ������ ������ ������� ���������, ���� ��� ����.
	animals.clear();

	// ��������� �������� ����, ���� �� ����.
	if (clan::FileHelp::file_exists(filename + "b")) {

		clan::File binFile(filename + "b",				// � ��������� ����� ���������� ����� 'demib'.
			clan::File::OpenMode::open_existing,
			clan::File::AccessFlags::access_read,
			clan::File::ShareFlags::share_read);

		// ������ �����.
		auto &strVer = binFile.read_string_nul();
		if (strVer != "Ver:1")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinVer), strVer));

		// ���������� ���������.
		auto &strElemCount = binFile.read_string_nul();
		auto &strElemCountAwait = "ElementsCount:" + clan::StringHelp::int_to_text(elemCount);
		if (strElemCount != strElemCountAwait)
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemCount), strElemCountAwait, strElemCount));

		// �������� ���������.
		for (int i = 0; i < elemCount; ++i) {
			auto &elemName = binFile.read_string_nul();
			if (elemName != arResNames[i])
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemName), arResNames[i], elemName));
		}

		// ������ ������ ������ ����� ����������.
		auto &strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Species:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Species:", strSecMarker));

		// ��������� ���� ���������� ����������.
		species = doReadSpecies(binFile, nullptr);

		// ������ ������ ������� ����� ���������� � ��� �������.
		std::set<std::string> speciesNamesDict;
		strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "SpecDictionary:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "SpecDictionary:", strSecMarker));
		unsigned int dictCnt = binFile.read_uint32();
		for (unsigned int i = 0; i < dictCnt; ++i)
			speciesNamesDict.insert(binFile.read_string_nul());

		// ������ ������ ������� �����.
		strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Dots:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Dots:", strSecMarker));

		// ��������� ����� �������� ����. ������ ����� ��������� ��� ���������� ����������.
		int dotsCount = worldSize.width * worldSize.height;
		int dotSize = Dot::getSizeInMemory();
		for (int i = 0; i < dotsCount; ++i) {

			// ������� �����.
			Dot &dot = arDots[i];

			// ���������� ��������� � �����.
			if (binFile.read(dot.res, dotSize) != dotSize)
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadDots), i));

			// �������� � �����.
			dot.organism = doReadOrganism(binFile, speciesNamesDict, getDotXYFromIndex(i));
		}

		// ������ ������ ������� ������������.
		auto &strResMaxMarker = binFile.read_string_nul();
		if (strResMaxMarker != "ResMax:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinResmaxMarker), strResMaxMarker));

		// ��������� ������������ ������������.
		dotSize = elemCount * sizeof(unsigned long long);
		if (binFile.read(arResMax, dotSize) != dotSize)
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadResmax)));

		// ������� ����.
		binFile.close();
	}

	// �������������� ������ ��� �������� ���������� ��������� ������� ������� � ���������� ������ �����.
	amounts.Init();
}

// ����������� ������� ��� ���������� ����� ����������.
std::shared_ptr<demi::Species> World::doReadSpecies(clan::File &binFile, std::shared_ptr<demi::Species> ancestor)
{
	// ��������� ������� ���.

	// ������ ��� � ������������� �������� ����.
	std::shared_ptr<demi::Species> retVal = std::make_shared<demi::Species>();
	retVal->ancestor = ancestor;
	retVal->name = binFile.read_string_nul();
	retVal->author = binFile.read_string_nul();
	retVal->visible = binFile.read_int8() != 0;
	unsigned char r = binFile.read_uint8(), g = binFile.read_uint8(), b = binFile.read_uint8();
	retVal->aliveColor = clan::Color(r, g, b);
	r = binFile.read_uint8(); g = binFile.read_uint8(); b = binFile.read_uint8();
	retVal->deadColor = clan::Color(r, g, b);
	
	// ���������������� �������.
	std::string reactionName = binFile.read_string_nul();
	retVal->reaction = reactions[reactionName];

	// ������ � ��������� ������.

	// ���������� ������.
	int64_t cnt = binFile.read_int64();
	
	// � ����� ������ ��� ������.
	for (int i = 0; i < cnt; ++i) {

		// ��� ������.
		int64_t cellType = binFile.read_int64();

		// ������ ������ ������� ����.
		std::shared_ptr<demi::GenericCell> cell;
		switch (cellType) {
		case 0 :	// cellBrain ����
			break;
		case 1:		// cellReceptor ��������
			break;
		case 2:		// cellMuscle �����
			break;
		case 3:		// cellAdipose ���
			break;
		case 4:		// cellAbdomen �����
			cell = std::make_shared<demi::CellAbdomen>();
			break;
		case 5:		// cellMouth ���
			break;
		case 6:		// cellArmor �����
			break;
		}

		// ��������� ������ � ��� ���������.
		retVal->cells.push_back(cell);
	}


	// ��������� �������� ����.

	// ���������� �������� �����.
	cnt = binFile.read_int64();

	// ��������� � ��������� �������� ����������.
	for (int i = 0; i < cnt; ++i)
		retVal->descendants.push_back(doReadSpecies(binFile, retVal));
	
	// ���������� ��������� �������.
	return retVal;
}

// ����������� ������� ��� ������ ����� ����������, ����������� ������ ������� �������� �����.
void World::doWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies, std::set<std::string> &dict)
{
	// ���������� �������� ����.
	binFile.write_string_nul(aSpecies->name);
	binFile.write_string_nul(aSpecies->author);
	binFile.write_int8(aSpecies->get_visible());

	// ����� �����������.
	binFile.write_uint8(aSpecies->aliveColor.get_red());
	binFile.write_uint8(aSpecies->aliveColor.get_green());
	binFile.write_uint8(aSpecies->aliveColor.get_blue());
	binFile.write_uint8(aSpecies->deadColor.get_red());
	binFile.write_uint8(aSpecies->deadColor.get_green());
	binFile.write_uint8(aSpecies->deadColor.get_blue());

	binFile.write_string_nul(aSpecies->reaction->name);

	// ��������� �������� � �������.
	dict.insert(aSpecies->getAuthorAndNamePair());

	// ���������� ������.
	binFile.write_int64(aSpecies->cells.size());
	for (auto& cell : aSpecies->cells) {
		// ��� ������.
		int64_t cellType = cell->getCellType();
		binFile.write_int64(cellType);
	}

	// ���������� �������� ����.

	// ���������� �������� �����.
	binFile.write_int64(aSpecies->descendants.size());

	// ���� ���� ����������.
	for (auto &spec : aSpecies->descendants)
		doWriteSpecies(binFile, spec, dict);
}


void World::saveModel(const std::string &filename)
{
	// ��������� ������

	// ���� ����� ��� �������, ���� ��� �������������.
	std::unique_lock<std::mutex> lock(threadMutex);
	bool prevRun = threadRunFlag;
	
	if (threadRunFlag) {
		// ������ �� ���������.
		threadRunFlag = false;
		threadEvent.notify_all();

		// ��� ���������.
		do {
			lock.unlock();
			clan::System::sleep(100);
			lock.lock();
		} while (threadRunFlag);
	}

	// ���������, �������� � XML-�����.
	auto pResDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// ��������� ������� ��� �������.
	clan::DomElement &prop = pResDoc->get_resource(cResGlobalsAppearance).get_element();
	prop.set_attribute_int(cResGlobalsAppearanceLeft, appearanceTopLeft.x);
	prop.set_attribute_int(cResGlobalsAppearanceTop, appearanceTopLeft.y);
	prop.set_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// ������� ��������� ��������������.
	prop = pResDoc->get_resource(cResGlobalsLUCA).get_element();
	prop.set_attribute_bool(cResGlobalsLUCAVisibility, species->visible);

	// ���������� �����.
	prop = pResDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, timeModel.year);
	prop.set_attribute_int(cResGlobalsTimeDay, timeModel.day);
	prop.set_attribute_int(cResGlobalsTimeSecond, timeModel.sec);

	// ������� ��������� ���������.
	for (int i = 0; i < elemCount; ++i) {

		// �������.
		prop = pResDoc->get_resource(cResElementsSection + std::string("/") + arResNames[i]).get_element();

		// ��������� ��������.
		prop.set_attribute_bool(cResElementsVisibility, arResVisible[i]);
	}

	// ���������� ��������� �� ����.
	pResDoc->save(filename);

	// �������� ���� ��� �����, ��������� � �.�.
	clan::File binFile(filename + "b",				// � ��������� ����� ���������� ����� 'demib'.
		clan::File::OpenMode::create_always,
		clan::File::AccessFlags::access_write,
		clan::File::ShareFlags::share_read);

	// ������� ������ �����.
	binFile.write_string_nul("Ver:1");
	
	// ������� ���������� ���������.
	binFile.write_string_nul("ElementsCount:" + clan::StringHelp::int_to_text(elemCount));

	// ������� �������� ���������.
	for (int i = 0; i < elemCount; ++i)
		binFile.write_string_nul(arResNames[i]);

	// ���������� ������ ����������.
	binFile.write_string_nul("Species:");

	// ���������� ���� ���������� ����������, ����������� �������� ������� �������� �����.
	std::set<std::string> speciesNamesDict;
	doWriteSpecies(binFile, species, speciesNamesDict);

	// ���������� ������ ������ ������� ����� ���������� � ��� �������.
	binFile.write_string_nul("SpecDictionary:");
	binFile.write_uint32(speciesNamesDict.size());
	for each (auto & item in speciesNamesDict)
		binFile.write_string_nul(item);

	// ���������� ������ ������ ������� �����.
	binFile.write_string_nul("Dots:");

	// ���������� �����.
	int dotsCount = worldSize.width * worldSize.height;
	int dotSize = Dot::getSizeInMemory();
	for (int i = 0; i < dotsCount; ++i) {
		// ������� �����.
		const Dot &dot = arDots[i];

		// ���������� ��������� � �����.
		binFile.write(dot.res, dotSize);

		// �������� � �����.
		doWriteOrganism(binFile, speciesNamesDict, dot.organism);
	}

	// ���������� ������ ������ ������� ����������.
	binFile.write_string_nul("ResMax:");

	// ���������� ��������� ������������ ��������.
	binFile.write(arResMax, elemCount * sizeof(unsigned long long));

	// ������� ����.
	binFile.close();

	// ��������� ���������� ������, ���� �� �������.
	if (prevRun) {
		threadRunFlag = true;
		threadEvent.notify_all();
	}
}


void World::workerThread()
{
	// ������� ������� ������, ������������ ������.
	try
	{
		while (true)
		{
			// ��������������� �� ��������� ������, �������� ��� ������������� �������� �����.
			std::unique_lock<std::mutex> lock(threadMutex);
			threadEvent.wait(lock, [&]() { return threadRunFlag || threadExitFlag; });

			// ���� ���� ������� �� �����, ��������� ������.
			if (threadExitFlag)
				break;

			// ������������ �������� �����.
			lock.unlock();

			// ��������� ������ ������.
			makeTick();

			// ��������� �������� ����� ��� ���������� ����������.
			lock.lock();

			// �������� ��������� ������ � �����, ������������ ��� �����������.
			timeBackup = timeModel;

			//throw clan::Exception("Bang!");	// <--- Use this to test the application handles exceptions in threads

			// ��� �������� ����� �������������� ��� ���������� ����� {}
		}
	}
	catch (clan::Exception &)
	{
		// ������� �� ��������� ���������� ������. ����� ������ ������� �����.
		std::unique_lock<std::mutex> lock(threadMutex);
		threadCrashedFlag = true;
	}
}

void World::runEvolution(bool is_active)
{
	// ���������������� ��� ���������� ������ ������
	std::unique_lock<std::mutex> lock(threadMutex);
	threadRunFlag = is_active;
	threadEvent.notify_all();
}

// �������� ������ ������.
void World::resetModel(const std::string &modelFilename, const std::string &defaultFilename)
{
	// ������� �������� ����.
	if (clan::FileHelp::file_exists(modelFilename + "b"))
		clan::FileHelp::delete_file(modelFilename + "b");

	// ���������� ��������� ������.
	try {
		globalWorld.loadModel(modelFilename);
		timeModel = DemiTime();
		timeBackup = timeModel;
	}
	catch (...) {
		// ��� ������ ��������� ������ ������.
		globalWorld.loadModel(defaultFilename);
	}
}

// �������� �� SaveModel() ��� ��������. ������ ������ ���������.
void World::doWriteOrganism(clan::File &binFile, std::set<std::string> &dict, demi::Organism* organism)
{
	// ���� ��������� ���, � �������� ����� ���� ������� -1.
	if (organism == nullptr) {
		binFile.write_int16(-1);
		return;
	}

	// ������� ��� ��������� � ������� � ���������� ��� ������.
	auto it = dict.find(organism->get_species()->getAuthorAndNamePair());
	INT16 index = std::distance(dict.begin(), it);
	binFile.write_int16(index);

	// ����, ��������� �������, ����� �������.
	binFile.write_uint8(uint8_t(organism->angle));
	binFile.write_int32(organism->getVitality());
	binFile.write_int32(organism->getFissionBarrier());

	// ���������� ����� �������.
	int cnt = organism->leftReagentAmounts.size();
	binFile.write(organism->leftReagentAmounts.data(), sizeof(unsigned long long) * cnt);
}

demi::Organism* World::doReadOrganism(clan::File &binFile, std::set<std::string> &dict, const clan::Point &center)
{
	int dictKey = binFile.read_int16();

	// ���� � �������� ����� -1, ������ ��������� � ����� ���.
	if (dictKey == -1)
		return nullptr;

	// ��������� ���������� ����.
	int angle = binFile.read_uint8();
	int vitality = binFile.read_int32();
	int fissionBarrier = binFile.read_int32();

	// �������� ���� � ������ ����� ������� �� ������� �� �������.
	auto it = dict.begin();
	std::advance(it, dictKey);
	std::string fullSpeciesName = *it;

	// �� �������� ���� ����� ������ �� ���. ���� ��� �������������, �� ��������� ���������� �������� shared_ptr, ��� ��� � ����������
	// species ���� ������ ������ ��� ��� ��������.
	std::shared_ptr<demi::Species> Aspecies = fullSpeciesName == species->getAuthorAndNamePair() ? species : species->getSpeciesByFullName(fullSpeciesName);

	// ������ ��������.
	demi::Organism* retVal = new demi::Organism(Aspecies, center, angle, vitality, fissionBarrier);

	// ���������� ����� �������.
	int cnt = retVal->leftReagentAmounts.size();
	binFile.read(retVal->leftReagentAmounts.data(), sizeof(unsigned long long) * cnt);

	// ���� ��������� ������� ������������, �������� �������� � ������ �����.
	if (retVal->isAlive())
		animals.push_back(retVal);

	return retVal;
}
