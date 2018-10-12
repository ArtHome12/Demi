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


const size_t cGeothermRadius = 12;

// ��������� ��� ������ XML-����� ������
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

auto cResAreaRect = "rect";	// ��� ��� ������������� ������� ���������� ������������� �������/����������.
auto cResAreaPoint = "point";	// ��� ��� �����.

auto cSolar = "Solar";			// ����������� ��������� �������.
auto cEnergy = "Geothermal";	// ����������� ������������� �������.

// ��������� �������
auto cWrongSize = "WorldWrongSize";
auto cWrongLUCAReaction = "WrongLUCAReaction";
auto cWrongBinVer = "WorldWrongBinaryFileVersion";
auto cWrongBinElemCount = "WorldWrongBinaryFileElementsCount";
auto cWrongBinElemName = "WorldWrongBinaryFileElementsName";
auto cWrongBinMarker = "WorldWrongBinaryFileMarker";
auto cWrongBinCannotReadDots = "WorldWrongBinaryFileCannotReadDots";

// ���������� ������ - ������� ���.
World globalWorld;

// =============================================================================
// �������� ��������� �������
// =============================================================================
void Solar::Shine(const demi::DemiTime &timeModel)
{
	// �������� �������� ��� ���������� ������� ������� �����.
	//
	// ����� ������ ������������ ������ ��� ���� ������ ��� �������������, �� ����������� ��� ���������� ����.

	// ��������� ������� ��������� � ���������� ������ � ������.
	LocalCoord coord(getPos(timeModel));

	int lightRadius = int(globalWorld.getLightRadius());
	for (int x = -lightRadius + 1; x != lightRadius; ++x)			// ����������� ����������� ���������� ����� ����� (�������) � ����� ������ (������) ��� ��������� - ����� ���� 1 ����� ��������� ���� ��� ��������������.
		for (int y = -lightRadius + 1; y != lightRadius; ++y) {

			// ����������� ���������� �� ������ ���������.
			double r = sqrt(x*x + y*y);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			r = r < lightRadius ? float((lightRadius - r) / lightRadius) : 0.0f;
			
			coord.get_dot(x, y).setSolarEnergy(float(r));
		}
}


clan::Point Solar::getPos(const demi::DemiTime &timeModel)
{
	// ���������� ������� ��� ������ � ����������� �� �������.

	// ������� ����.
	const clan::Size &worldSize = globalWorld.get_worldSize();

	// ������� �� ����������� ������� �� ������� �����.
	// ������ ��������� � ������� �� ����� ��������������� ��������� ���� �����.
	int x = int(worldSize.width * (1.0f - float(timeModel.sec) / (demi::cTicksInDay - 1.0f)) + 0.5f);

	// ������� �� ��������� ������� �� ��� ����.
	// ������ ������� ��������� �� ����� ������� �������� �� ������ � ������� � �������� �����������, 
	// �� ���� ����� ������� ������� �����������.
	
	// �������� ����, ��� ��������.
	const int halfYear = int(demi::cDaysInYear / 2 + 0.5f);

	// ������ ��������
	const int tropic = int(globalWorld.getTropicHeight());

	// ����� ��� ������ �������� ����, ���� �� �������� �������� ����, � ����� ������ - ����������.
	int y = int((int(timeModel.day) < halfYear ? (worldSize.height - tropic) / 2.0f + int(timeModel.day) * tropic / halfYear : (worldSize.height + tropic) / 2.0 - (int(timeModel.day) - halfYear) * tropic / halfYear) +0.5);

	return clan::Point(x, y);
}



// =============================================================================
// ������ �����������, ��������� ������ �����.
// =============================================================================
World::World() : generator(random_device()), rnd_angle(0, 7), rnd_Coord(0, 12 - 1), thread(std::thread(&World::workerThread, this))

{
	// ����� �� �������� �� ��������� �����.
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
	size_t cnt = animals.size();
	for (size_t i = 0; i < cnt; ++i) {

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

	// ������ ��������� ��������������, ���� ��� ����� ���������� � ���� �����.
	if (!cnt) {
		Dot& protoDot = LocalCoord(LUCAPos).get_dot(0, 0);
		if (protoDot.organism == nullptr)
			animals.push_back(new demi::Organism(LUCAPos, 0, 1, getModelTime(), 0, genotypesTree.species.front()));
	}
}



void World::fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect)
{
	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	//
	for (size_t x = rect.left; x != rect.right; ++x)
		for (size_t y = rect.top; y != rect.bottom; ++y)
			arDots[getDotIndexFromXY(x, y)].setElementAmount(resId, amount);
}


// �������� ��������.
void World::diffusion()
{
	// �������� ������� arResMax, ������������ ��� ������ ����� ����� �� ������ ������������ ������������������ �������� � ���, �� ������ ������ ������ �����.
	// � �����-�� �������������� ���� �� ���������������.���� ������ ��� ��� ����������, ���� ��� ��������, ���� ����� �� �������, �� �������� ���������� ������ ��������������� ��������� �������� ���� ��� ����������������
	// ��������� ������.���� ������ ����� ����������� ��������� ����� �������, � ����� ����� ��������� �����, ��� ����� ���������� ��� ���. ������� ������ ��� ������ ���� ������������ ������������ ������� �� �������.
	for (size_t i = 0; i != elemCount; ++i)
		if (arResMax[i])
			--arResMax[i];


	// ������� �������, �� ������� �������� ������.
	Dot *cur = arDots;											// �������� ����� ��� �������� �������� - ������ ����� �������.
	Dot *last = cur + worldSize.width * worldSize.height;		// ����� ����� ��������� ����� �������.
	Dot *dest;													// �������� �����

	// �������
	std::uniform_int_distribution<> rnd_Elem(0, elemCount-1);

	while (true) {

		size_t rnd = rnd_angle(generator);			// �� 0 �� 7, ����������� ��������
		const size_t rndResIndex = rnd_Elem(generator);		// ��������� �������.

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
		if (!amount && fromDot.res[rndResIndex])
			amount = 1;

		fromDot.res[rndResIndex] -= amount;
		toDot.res[rndResIndex] += amount;

		// ������� ���������.
		//
		if (arResMax[rndResIndex] < toDot.res[rndResIndex])
			arResMax[rndResIndex] = toDot.res[rndResIndex];

		// ��������� ���������.
		demi::Organism* curOrganism = fromDot.organism;
		if (!curOrganism)
			continue;

		// ���� � �������� ����� ���� �������� � ��� ���� �������, ������� ���.
		if (curOrganism->needDesintegration()) {
			// �� ������� ����� �������� ������ ��� ���� � �����������.
			delete curOrganism;
		}
		else {
			// ���� � �������� ����� ��� ���������, �� ���������� ��������� ��������.
			if (toDot.organism == nullptr && curOrganism->canMove()) {
				// ���������� ����� �����.
				curOrganism->moveTo(getDotXYFromIndex(dest - arDots));

				// ��������� ��������� �������, ����������� �� �������.
				curOrganism->processInactiveVitality();
			} else {
				// ���� �������� ����, �� ��������� ��� �������, ����� �� ��������.
				if (!curOrganism->isAlive())
					curOrganism->processInactiveVitality();
			}
		}
	}
}

// ���������� ���������� ����� �� ���������� �������.
clan::Point World::getDotXYFromIndex(size_t index)
{
	int y = int(index / worldSize.width);
	int x = index - y * worldSize.width;
	return clan::Point(x, y);
}


void World::addGeothermal(size_t i, const clan::Point &coord)
{
	// ����� �������������� ���������� ������������� �������.
	//

	arEnergy[i].coord = coord;

	// ��� ��� ������������� ��������� �� ������ ������ ������� �������������, ����� �� ������� ������������� �������.
	//

	// ��������� ������� ���������.
	LocalCoord geothermalCoord(coord);
	int rad = cGeothermRadius;
	for (int xp = -rad; xp <= rad; ++xp)
		for (int yp = -rad; yp <= rad; ++yp) {

			// ����������� ���������� �� ������ ���������.
			double r = sqrt(xp*xp + yp*yp);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			if (r < rad)
				geothermalCoord.get_dot(xp, yp).setGeothermalEnergy(float((rad - r) / rad));
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

	// ����� � ������� ��� ����������� ������� ���� ���.
	const std::vector<std::string>& elementNames = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);
	const std::vector<std::string>& reactionNames = resDoc->get_resource_names_of_type(cResReactionsType, cResReactionsSection);

	// �������� ��������� � ������� �������.
	doLoadInanimal(resDoc, elementNames, reactionNames);

	// �������� ����� �������.
	doLoadAnimal(resDoc, filename, reactionNames);

	// �������������� ������ ��� �������� ���������� ��������� ������� ������� � ���������� ������ �����.
	amounts.Init();
}

// ��� ��������� ������� loadModel, saveModel.
void World::doLoadInanimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc, const std::vector<std::string>& elementNames, const std::vector<std::string>& reactionNames)
{
	// �������������� ������� ����.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// ������ ����.
	worldSize.width = prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000);

	// ������ ����
	worldSize.height = int(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f) + 0.5f);
	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongSize), worldSize.width, worldSize.height));

	// ������ ���������� ����� � ������ �������� ������� �� ������� ����.
	lightRadius = size_t(0.9f * worldSize.height / 2 + 0.5f);
	tropicHeight = size_t(0.2f * worldSize.height + 0.5f);

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

	// ��������� �������.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);

	// ���������� ����������.
	energyCount = energy.size();

	// ���������� ��������.
	elemCount = elementNames.size();

	// ������� ������ ��� �������.
	arDots = new Dot[worldSize.width * worldSize.height];	// ����� �����������.
	arResColors = new clan::Color[elemCount];				// ����� ���������.
	arResMax = new unsigned long long[elemCount];			// ������������ ������������ ��������� � ����� �����.
	arResNames = new std::string[elemCount];				// �������� ���������.
	arResVisible = new bool[elemCount];						// ��������� ���������.
	arResVolatility = new float[elemCount];					// ��������� ���������.
	arEnergy = new Geothermal[energyCount];					// ������������� ���������.

															// ��������� �������� ���������.
	for (size_t i = 0; i != elemCount; ++i) {

		// �������.
		clan::XMLResourceNode &res = resDoc->get_resource(elementNames[i]);

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
	for (size_t i = 0; i != energyCount; ++i) {
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
	reactions.clear();
	for (auto & reactionName : reactionNames) {
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
			auto pos = std::distance(elementNames.begin(), find(elementNames.begin(), elementNames.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->leftReagents.push_back(reagent);
		}

		// �������� ������.
		nodes = prop.get_elements_by_tag_name(cResReactionsRightReagent);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// �� �������� �������� ������ �������� � ��������� ��� � ������� ������ � �����������.
			auto pos = std::distance(elementNames.begin(), find(elementNames.begin(), elementNames.end(), cResElementsSection + std::string("/") + reagentName));
			demi::ReactionReagent reagent(pos, node.get_attribute_int(cResReactionsAmount));
			curReaction->rightReagents.push_back(reagent);
		}

		// ��������� ������� � ������ �������.
		reactions.push_back(curReaction);
	}

	// �������������� ������ ���������� �� ������ ��������� ��������� � ������, ������������ ����� ��������.
	InitResMaxArray();
}


void World::doLoadAnimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc, const std::string &filename, const std::vector<std::string>& reactionNames)
{
	// ������ ��� ��������������.
	//
	// ��������� ��������� ��� ����������.
	auto prop = resDoc->get_resource(cResGlobalsMetabolicConsts).get_element();
	demi::Organism::minActiveMetabolicRate = uint8_t(prop.get_attribute_int(cResGlobalsMCminActiveMetabolicRate));
	demi::Organism::minInactiveMetabolicRate = uint8_t(prop.get_attribute_int(cResGlobalsMCminInactiveMetabolicRate));
	demi::Organism::desintegrationVitalityBarrier = int32_t(prop.get_attribute_int(cResGlobalsMCdesintegrationVitalityBarrier));

	// ��������� ��� ��������������.
	prop = resDoc->get_resource(cResGlobalsLUCA).get_element();
	bool specVisible = prop.get_attribute_bool(cResGlobalsLUCAVisibility);
	//uint16_t specFissionBarrier = prop.get_attribute_int(cResGlobalsLUCAFissionBarier);
	clan::Color specAliveColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCAAliveColor)));
	clan::Color specDeadColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCADeadColor)));
	LUCAPos = clan::Point(prop.get_attribute_int("x"), prop.get_attribute_int("y"));
	const std::string LUCAGeneName = prop.get_attribute("geneName");
	const std::string LUCAReactionName = prop.get_attribute("geneValue");

	// ��������� ������ ������� �������������� (�������� ��� �������� ������� ���� � ��������� ������).
	auto itReaction = std::find(reactionNames.begin(), reactionNames.end(), std::string(cResReactionsSection) + "/" + LUCAReactionName);
	if (itReaction == reactionNames.end())
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongLUCAReaction), LUCAReactionName));
	demi::geneValues_t geneValue = demi::geneValues_t(std::distance(reactionNames.begin(), itReaction));

	// ������� ������ ������ ���������/�����.
	genotypesTree.clear();

	// ������ ��� �������.
	demi::Gene gene(LUCAGeneName, reactionNames);

	// ������ �������� ������� � ����������� ��� ������ ������.
	genotypesTree.genotype = std::make_shared<demi::Genotype>(nullptr, gene, "LUCA", "Demi");

	// ������ ��� ����������� ������������� ��� ��������������.
	auto species = std::make_shared<demi::Species>(genotypesTree.genotype, geneValue, specVisible, specAliveColor, specDeadColor);
	species->getCellsRef().push_back(std::make_shared<demi::CellAbdomen>());

	// ��������� �������� ��� � ������.
	genotypesTree.species.push_back(species);

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
		auto &strElemCountAwait = "ElementsCount:" + clan::StringHelp::int_to_text(int(elemCount));
		if (strElemCount != strElemCountAwait)
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemCount), strElemCountAwait, strElemCount));

		// �������� ���������.
		for (size_t i = 0; i != elemCount; ++i) {
			auto &elemName = binFile.read_string_nul();
			if (elemName != arResNames[i])
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinElemName), arResNames[i], elemName));
		}

		// �������� ��� ������ ������ ������� ����� ��� ����, ����� ��� ���������� ���������� ������������ ���� ���� �������.
		// ���� ����� ���� ��������� ���������.
		speciesDict_t speciesDict;
		genotypesTree.generateDict(speciesDict);

		// ������ ������ ������� �����.
		auto strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Dots:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Dots:", strSecMarker));

		// ��������� ����� �������� ����. ������ ����� ��������� ��� ���������� ����������.
		size_t dotsCount = worldSize.width * worldSize.height;
		for (size_t i = 0; i != dotsCount; ++i) {

			// ������� �����.
			Dot &dot = arDots[i];

			// ���������� ��������� � �����.
			const size_t elemArraySize = sizeof(uint64_t) * elemCount;
			if (binFile.read(dot.res, elemArraySize) != elemArraySize)
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadDots), i));

			// �������� � �����.
			dot.organism = doReadOrganism(binFile, speciesDict, getDotXYFromIndex(i));
		}

		// ������� ����.
		binFile.close();
	}
}


/*// ����������� ������� ��� ���������� ����� ����������. 
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

	// ��������� ����� ����������� (����� �������� ��-�� ������������).
	retVal->fissionBarrier = binFile.read_uint32();

	// ������ � ��������� ������.

	// ���������� ������.
	uint64_t cnt = binFile.read_uint64();
	
	// � ����� ������ ��� ������.
	for (int i = 0; i < cnt; ++i) {

		// ��� ������.
		uint64_t cellType = binFile.read_uint64();

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
	cnt = binFile.read_uint64();

	// ��������� � ��������� �������� ����������.
	for (uint64_t i = 0; i != cnt; ++i)
		retVal->descendants.push_back(doReadSpecies(binFile, retVal));
	
	// ���������� ��������� �������.
	return retVal;
}

// ����������� ������� ��� ������ ����� ����������, ����������� ������ ������� �������� �����.
void World::doWriteSpecies(clan::File &binFile, std::shared_ptr<demi::Species> aSpecies, std::set<std::string> &dict)
{
	// ���������� �������� ����.
	binFile.write_string_nul(aSpecies->getName());
	binFile.write_string_nul(aSpecies->getAuthor());
	binFile.write_int8(aSpecies->getVisible());

	// ����� �����������.
	const clan::Color& aliveColor = aSpecies->getAliveColor();
	const clan::Color& deadColor = aSpecies->getDeadColor();
	binFile.write_uint8(aliveColor.get_red());
	binFile.write_uint8(aliveColor.get_green());
	binFile.write_uint8(aliveColor.get_blue());
	binFile.write_uint8(deadColor.get_red());
	binFile.write_uint8(deadColor.get_green());
	binFile.write_uint8(deadColor.get_blue());

	binFile.write_string_nul(aSpecies->reaction->name);

	// ��������� �������� � �������.
	dict.insert(aSpecies->getAuthorAndNamePair());

	// ��������� ����� �����������.
	binFile.write_uint32(aSpecies->fissionBarrier);

	// ���������� ������.
	binFile.write_uint64(aSpecies->cells.size());
	for (auto& cell : aSpecies->cells) {
		// ��� ������.
		uint64_t cellType = cell->getCellType();
		binFile.write_uint64(cellType);
	}

	// ���������� �������� ����.

	// ���������� �������� �����.
	binFile.write_uint64(aSpecies->descendants.size());

	// ���� ���� ����������.
	for (auto &spec : aSpecies->descendants)
		doWriteSpecies(binFile, spec, dict);
}*/


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
	prop.set_attribute_bool(cResGlobalsLUCAVisibility, genotypesTree.species.front()->getVisible());

	// ���������� �����.
	prop = pResDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, int(timeModel.year));
	prop.set_attribute_int(cResGlobalsTimeDay, int(timeModel.day));
	prop.set_attribute_int(cResGlobalsTimeSecond, int(timeModel.sec));

	// ������� ��������� ���������.
	for (size_t i = 0; i != elemCount; ++i) {

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
	binFile.write_string_nul("ElementsCount:" + clan::StringHelp::int_to_text(int(elemCount)));

	// ������� �������� ���������.
	for (size_t i = 0; i != elemCount; ++i)
		binFile.write_string_nul(arResNames[i]);

	// ��� ����, ����� �� ������ ����� ������ ���������� ������ � ��� ��������� ��������, ������� ������� � ����� ������������ �������.
	speciesDict_t speciesDict;
	genotypesTree.generateDict(speciesDict);

	// ���������� ������ ������ ������� �����.
	binFile.write_string_nul("Dots:");

	// ���������� �����.
	size_t dotsCount = worldSize.width * worldSize.height;
	const size_t elemArraySize = sizeof(uint64_t) * elemCount;
	for (size_t i = 0; i != dotsCount; ++i) {
		// ������� �����.
		const Dot &dot = arDots[i];

		// ���������� ��������� � �����.
		binFile.write(dot.res, elemArraySize);

		// �������� � �����.
		doWriteOrganism(binFile, speciesDict, dot.organism);
	}

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
		timeModel = demi::DemiTime();
		timeBackup = timeModel;
	}
	catch (...) {
		// ��� ������ ��������� ������ ������.
		globalWorld.loadModel(defaultFilename);
	}
}

// �������� �� SaveModel() ��� ��������. ������ ������ ���������.
void World::doWriteOrganism(clan::File &binFile, speciesDict_t& speciesDict, demi::Organism* organism)
{
	// ���� ��������� ���, � �������� ����� ���� ������� UINT16_MAX.
	if (!organism) {
		binFile.write_uint16(UINT16_MAX);
		return;
	}

	// ������� ��������� �� ��� ��������� � ������� � ���������� ��� ������. �������� �� ������� ���� � ������� �� ������.
	auto it = std::find(speciesDict.begin(), speciesDict.end(), organism->getSpecies());
	ptrdiff_t index = std::distance(speciesDict.begin(), it);
	binFile.write_uint16(uint16_t(index));

	// ���������� ���� ���� (�������� �����, ��������� � �����).
	organism->getSpecies()->saveToFile(binFile);

	// ���������� ����������� ���� ���������.
	organism->saveToFile(binFile);
}

demi::Organism* World::doReadOrganism(clan::File &binFile, speciesDict_t& speciesDict, const clan::Point &center)
{
	uint16_t dictKey = binFile.read_uint16();

	// ���� � �������� ����� UINT16_MAX, ������ ��������� � ����� ���.
	if (dictKey == UINT16_MAX)
		return nullptr;

	// ��������� �� ��� ������� �� ������� �� �������.
	auto it = speciesDict.begin();
	std::advance(it, dictKey);
	const std::shared_ptr<demi::Species>& species = *it;

	// �������� �������� �� �����.
	demi::Organism* retVal = demi::Organism::createFromFile(binFile, center, species);

	// ���� ��������� ������� ������������, �������� �������� � ������ �����.
	if (retVal->isAlive())
		animals.push_back(retVal);

	return retVal;
}

// �������������� ������ ���������� �� ������ ��������� ��������� � ������, ������������ ����� ��������.
void World::InitResMaxArray()
{
	// ������� ������ ��������.
	memset(arResMax, 0, sizeof(unsigned long long) * elemCount);

	// ���������� ��� ����� � ��������� ����������.
	Dot *cur = globalWorld.getDotsArray();						// ������ ����� �������.
	Dot *last = cur + worldSize.width * worldSize.height;		// ����� ����� ��������� ����� �������.
	while (cur < last) {

		// ���������� ��� �������� � �����.
		for (size_t j = 0; j != elemCount; ++j) {
			
			// ���������� �������� � �����.
			unsigned long long amnt = cur->getElemAmount(j);

			// ������������ ��� ������������� ��������.
			if (getResMaxValue(j) < amnt)
				arResMax[j] = amnt;
		}

		++cur;
	}
}
