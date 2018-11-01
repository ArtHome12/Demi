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
#include "local_coord.h"
#include "genotypes_tree.h"
#include "organism.h"
#include "settings_storage.h"
#include "reactions.h"


const size_t cGeothermRadius = 12;

// ��������� ��� ������ XML-����� ������
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

auto cResGenesSection = "Genes";				// ��������� � cResReactionsSection!
auto cResGenesType = "Gene";
auto cResGenesValue = "Value";
auto cResGenesValueName = "name";

auto cResReactionsSection = "Genes/Reaction";	// ��������� � cResGenesSection!
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

auto cResAreaRect = "rect";	// ��� ��� ������������� ������� ���������� ������������� �������/����������.
auto cResAreaPoint = "point";	// ��� ��� �����.

auto cSolar = "Solar";			// ����������� ��������� �������.
auto cEnergy = "Geothermal";	// ����������� ������������� �������.

// ��������� �������
auto cWrongSize = "WorldWrongSize";
auto cWrongResolution = "WorldWrongResolution";
auto cWrongReaction = "WrongReaction";
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
	demi::LocalCoord coord(getPos(timeModel));

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
	const clan::Size& worldSize = globalWorld.getWorldSize();
	 
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
World::World() : generator(random_device()), rnd_angle(0, 7), rnd_Coord(0, 12 - 1), thread(std::thread(&World::workerThread, this)), genotypesTree(std::make_shared<demi::GenotypesTree>())
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

	// ������� ���� ����� � ����������.
	processDots();

	// ������ ��������� ��������������, ���� ��� ����� ���������� � ���� �����.
	if (!genotypesTree->genotype->getAliveCount()) {
		demi::Dot& protoDot = demi::LocalCoord(LUCAPos).get_dot(0, 0);
		if (protoDot.organism == nullptr)
			new demi::Organism(LUCAPos, 0, 1, timeModel, 0, genotypesTree->species.front());
	}
	//runEvolution(false);
}


void World::fillRectResource(size_t resId, unsigned long long amount, const clan::Rect &rect)
{
	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	//
	for (size_t x = rect.left; x != rect.right; ++x)
		for (size_t y = rect.top; y != rect.bottom; ++y)
			arDots[getDotIndexFromXY(x, y)].setElemAmount(resId, amount);
}


// ������� ���� ����� � ����������.
void World::processDots()
{
	// �������� ������� arResMax, ������������ ��� ������ ����� ����� �� ������ ������������ ������������������ �������� � ���, �� ������ ������ ������ �����.
	// � �����-�� �������������� ���� �� ���������������.���� ������ ��� ��� ����������, ���� ��� ��������, ���� ����� �� �������, �� �������� ���������� ������ ��������������� ��������� �������� ���� ��� ����������������
	// ��������� ������.���� ������ ����� ����������� ��������� ����� �������, � ����� ����� ��������� �����, ��� ����� ���������� ��� ���. ������� ������ ��� ������ ���� ������������ ������������ ������� �� �������.
	for (size_t i = 0; i != elemCount; ++i)
		if (arResMax[i])
			--arResMax[i];


	// ������� �������, �� ������� �������� ������.
	demi::Dot *cur = arDots;											// �������� ����� ��� �������� �������� - ������ ����� �������.
	demi::Dot *last = cur + worldSize.width * worldSize.height;		// ����� ����� ��������� ����� �������.
	demi::Dot *dest;													// �������� �����

	// �������
	std::uniform_int_distribution<> rnd_Elem(0, elemCount-1);

	while (true) {

		// ���������� �������� ���������� ��������� ��������� ������.
		cur += rnd_Coord(generator);

		// ���� ���������� ��� �����, �����������.
		if (cur >= last)
			break;

		// ���������� �������� ���������� - � ���� �� ������ �� ��������.
		size_t rnd = rnd_angle(generator);						// �� 0 �� 7, ����������� ��������
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
		demi::Dot &fromDot = *cur;
		demi::Dot &toDot = *dest;
		const size_t rndResIndex = rnd_Elem(generator);			// ��������� �������.
		unsigned long long amount = unsigned long long(fromDot.res[rndResIndex] * arResVolatility[rndResIndex] + 0.5f);

		// � ��������, ����� �������� � ����� ���� � ������������ �� ��������� ��� ����, ���������� 1 �������, ����� ��� ��������� ����������� ������� � ���������� ����� ����������.
		if (!amount && fromDot.res[rndResIndex])
			amount = 1;

		fromDot.res[rndResIndex] -= amount;
		toDot.res[rndResIndex] += amount;

		// ��������� ���������, ���� �� ����. ����� ������� � ���������� ��������� - � �� ����� ��� � ������� ������� �������� ���� �������, �������� � ��������.
		// ����� �������� � ����� ��������� ������ ����������� ������� �������� ����������� ������ � ����. 
		// ���� �� �������, ��������� � ��������� �����, � ��� �� ������� (������������ - ����� ������������). 
		// � ����� ������� ����� ��������� �������� ������ ������, � ������ - ������-������������ �������� ������ �� ������, �������� ���������, �� ������� ������� ������������.
		// � ������� ������� ��� ��������� ������������ � �������� � �� ���� ����������� ��������� ����� ������� �������� ��������.
		demi::Organism* curOrganism = fromDot.organism;
		if (curOrganism && curOrganism->tryLock()) {

			// ���� ������ �������� �������� � ��� ���� �������, ������� ���.
			if (curOrganism->needDesintegration()) {
				// �� ������� ����� �������� ������ ��� ���� � �����������.
				delete curOrganism;
			}
			else {
				
				// ������� ��������� ���������� � ���������� ��� ������� ������������.
				if (curOrganism->makeTick()) {

					// �������� ����� ���������� ��������� �����.
					clan::Point freePlace;
					if (curOrganism->findFreePlace(freePlace)) {

						// ������������� ��������� ���� � ��������� ��������� (����� �������� ���������).
						int32_t newVitality = curOrganism->halveVitality();

						// �� 0 �� 7, ����������� �������� ������ ���������.
						uint8_t newAngle = rnd_angle(generator);

						// ��� ������ ���������, �������� � �������� ���� ��� �� �����.
						const std::shared_ptr<demi::Species>& oldSpec = curOrganism->getSpecies();
						std::shared_ptr<demi::GenotypesTree> treeNode = oldSpec->getGenotype()->getTreeNode();
						const std::shared_ptr<demi::Species> newSpec = treeNode->breeding(oldSpec);

						// ���������� ������� � ������� �� ������������.
						uint64_t newAncestorCount = curOrganism->getAncestorsCount() + 1;
						if (newAncestorCount == UINT64_MAX)
							--newAncestorCount;

						// ���� ���� �������, �� ������� ������� ���������� �������, � ����� ������� ���� �����.
						if (oldSpec != newSpec)
							newAncestorCount = 0;

						// �������������� ������ ��������� �������� � ���������� ����������.
						clan::Point newPoint = curOrganism->getCenter().getGlobalPoint(freePlace);

						// ������ ����������� ��������. �� ������ �������������� �� �������� ��� ���� � ������������.
						new demi::Organism(newPoint, newAngle, newVitality, timeModel, newAncestorCount, newSpec);
					}
				}
				
				// ���� � �������� ����� ��� ���������, �� ���������� ��������� ��������.
				if (toDot.organism == nullptr && curOrganism->canMove()) {
					// ���������� ����� �����.
					curOrganism->moveTo(getDotXYFromIndex(dest - arDots));

					// ��������� ��������� �������, ����������� �� �������.
					curOrganism->processInactiveVitality();
				}
				else {
					// ���� �������� ����, �� ��������� ��� �������, ����� �� ��������.
					if (!curOrganism->isAlive())
						curOrganism->processInactiveVitality();
				}

				// ������������ ��������.
				curOrganism->unlock();
			}
		}

		// ������� ���������.
		//
		if (arResMax[rndResIndex] < toDot.res[rndResIndex])
			arResMax[rndResIndex] = toDot.res[rndResIndex];
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
	demi::LocalCoord geothermalCoord(coord);
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

	// �������� ��������� � ������� �������.
	doLoadInanimal(resDoc);

	// �������� �������.
	doLoadReactions(resDoc);

	// �������� ����� �������.
	doLoadAnimal(resDoc);

	// �������� �������� ����.
	doLoadBinary(filename);

	// �������������� ������ ���������� �� ������ ��������� ��������� � ������, ������������ ����� ��������.
	InitResMaxArray();

	// �������������� ������ ��� �������� ���������� ��������� ������� ������� � ���������� ������ �����.
	amounts.Init();
}

// ��� ��������� ������� loadModel, saveModel.
void World::doLoadInanimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
{
	// �������������� ������� ����.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// ������ ����.
	worldSize.width = prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000);

	// ������ ����
	worldSize.height = int(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f) + 0.5f);
	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongSize), worldSize.width, worldSize.height));

	// ����������� �����������, ��������� � ��������� � ��������� � ��� � ����������� ����� �� 1 �� 100.
	int resolution = prop.get_attribute_int(cResGlobalsWorldResolution);
	if (resolution == 0 || resolution > 100)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongResolution), resolution));

	resolution = int(100.0f / resolution + 0.5f);
	rnd_Coord = std::uniform_int_distribution<>(0, resolution);

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

	// ��������� �������.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);
	energyCount = energy.size();

	// ��������.
	const std::vector<std::string>& elementNames = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);
	elemCount = elementNames.size();

	// ������� ������ ��� �������.
	arDots = new demi::Dot[worldSize.width * worldSize.height];		// ����� �����������.
	arResColors = new clan::Color[elemCount];					// ����� ���������.
	arResMax = new unsigned long long[elemCount];				// ������������ ������������ ��������� � ����� �����.
	arResNames = new std::string[elemCount];					// �������� ���������.
	arResVisible = new bool[elemCount];							// ��������� ���������.
	arResVolatility = new float[elemCount];						// ��������� ���������.
	arEnergy = new Geothermal[energyCount];						// ������������� ���������.

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
}

void World::doLoadReactions(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
{
	// ��������� ���������� �������.
	reactions.clear();

	// ������� ������ ���� � ���������.
	clan::XMLResourceNode &res = resDoc->get_resource(cResReactionsSection);

	// ���������� ��� ��������, �� ���� ��� �������.
	clan::DomNodeList& reactionNodes = res.get_element().get_elements_by_tag_name(cResGenesValue);
	for (int i = reactionNodes.get_length() - 1; i >= 0; --i) {

		// ������� ���� � ����������� � �������.
		clan::DomElement &reactionNode = reactionNodes.item(i).to_element();

		// ������ �������.
		auto curReaction = std::make_shared<demi::ChemReaction>();

		// ���� ������� - ��������, ���������� ����������� ��� � ��������� ������� � ������ ��������� �������.
		curReaction->name = reactionNode.get_attribute(cResGenesValueName);
		curReaction->geoEnergy = reactionNode.get_attribute_float(cResReactionsGeoEnergy);
		curReaction->solarEnergy = reactionNode.get_attribute_float(cResReactionsSolarEnergy);
		curReaction->vitalityProductivity = reactionNode.get_attribute_int(cResReactionsVitalityProductivity);

		// �������� �����.
		clan::DomNodeList& leftNodes = reactionNode.get_elements_by_tag_name(cResReactionsLeftReagent);
		for (int i = leftNodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = leftNodes.item(i).to_element();

			// �������� (��������) ��������.
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");

			// ������ ������� � �������� ������ �������� � �����������.
			demi::ReactionReagent reagent((uint8_t)findElemIndex(reagentName), node.get_attribute_int(cResReactionsAmount));

			// ��������� ������� � �������.
			curReaction->leftReagents.push_back(reagent);
		}

		// �������� ������.
		clan::DomNodeList& rightNodes = reactionNode.get_elements_by_tag_name(cResReactionsRightReagent);
		for (int i = rightNodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = rightNodes.item(i).to_element();
			std::string reagentName = node.get_attribute(cResReactionsReagentName, "");
			demi::ReactionReagent reagent((uint8_t)findElemIndex(reagentName), node.get_attribute_int(cResReactionsAmount));
			curReaction->rightReagents.push_back(reagent);
		}

		// ��������� ������� � ������ �������.
		reactions.push_back(curReaction);
	}
}


void World::doLoadAnimal(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
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
	clan::Color specAliveColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCAAliveColor)));
	clan::Color specDeadColor = clan::Color(clan::Colorf(prop.get_attribute(cResGlobalsLUCADeadColor)));
	LUCAPos = clan::Point(prop.get_attribute_int("x"), prop.get_attribute_int("y"));
	
	// ��������� � ������������ �������, ���� �������.
	float mutationChance = prop.get_attribute_float(cResGlobalsLUCAMutationChance);

	// �������� ����� ���������, ����� � �� ���� ���������� ��������, ���������� �������� mutationChance.
	rnd_Mutation = std::uniform_int_distribution<>(0, int(1.0 / mutationChance));

	const std::string LUCAGeneName = prop.get_attribute("geneName");
	const std::string LUCAReactionName = prop.get_attribute("geneValue");

	// ������� ������ ������ ���������/�����. ����� ���� �������������� ��� ���������� ��������� �����.
	genotypesTree->clear();

	// ��������� ������ � ������.
	//
	// ������ ���� �������� �����.
	const std::vector<std::string>& geneNames = resDoc->get_resource_names_of_type(cResGenesType, cResGenesSection);

	// ��������� ��������� ��� ������ ����� (�������������� ����� ����������� � ��������).
	std::map<std::string, std::vector<std::string>> geneDatas;

	// ���������� ��� ������� � ������
	for (auto& geneName : geneNames) {

		// ��������� ��� ������� XML-�����.
		clan::XMLResourceNode &resGene = resDoc->get_resource(geneName);
		clan::DomElement &propGene = resGene.get_element();

		// ������ ��� �������� ��������.
		std::vector<std::string> tmpVect;

		// ���������� ����������� - �������� ��������.
		clan::DomNodeList& nodes = propGene.get_elements_by_tag_name(cResGenesValue);
		for (int i = nodes.get_length() - 1; i >= 0; --i) {
			clan::DomElement &node = nodes.item(i).to_element();
			// ��������� �������� � �������.
			tmpVect.push_back(node.get_attribute(cResGenesValueName, ""));
		}

		// ��������� ��� � ���������, � ��� ����� ��� ��� ��������.
		geneDatas.insert(std::pair<std::string, std::vector<std::string>>(resGene.get_name(), tmpVect));
	}

	// ������ ��� �������������� (��� �������) �� ������ �� �����. ������� ��������� ������ ����� ���������� � ��������, ��������� ������������.
	demi::Gene gene(LUCAGeneName, geneDatas.at(LUCAGeneName));

	// ������ ������� ��������������.
	genotypesTree->genotype = std::make_shared<demi::Genotype>(genotypesTree, gene, "LUCA", "Demi", specAliveColor, specDeadColor);

	// ��������� ������ ������� ��������������, �� ���� �������� ��� ����.
	// ��� ����� �������� ��� ������� � ������� ������. �� ���� ���������� ���������� ����� ���������� �� map.
	geneValues_t geneValue = geneValues_t_MAX;
	for (size_t i = 0; i != reactions.size(); ++i)
		if (reactions[i]->name == LUCAReactionName) {
			geneValue = (geneValues_t)i;
			break;
		}
	if (geneValue == geneValues_t_MAX)
		throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongLUCAReaction), LUCAReactionName));

	// ������ ��� ����������� ������������� ��� ��������������. ����� ���� ������������ ��� ���������� ��������� �����.
	auto species = std::make_shared<demi::Species>(genotypesTree->genotype, geneValue, true);

	// ��������� �������� ��� � ������.
	genotypesTree->species.push_back(species);

	// �������� ���������� ��������� ��������.
	auto genotypeNames = resDoc->get_resource_names_of_type(cResGenotypesType, cResGenotypesSection);
	for (auto& genotypeName : genotypeNames) {
		doLoadGenotypes(resDoc->get_resource(genotypeName).get_element(), genotypesTree, geneDatas);
	}
}

void World::doLoadGenotypes(clan::DomElement& node, std::shared_ptr<demi::GenotypesTree> ancestorTreeItem, std::map<std::string, std::vector<std::string>>& geneDatas)
{
	// ���� - ��������, �����, �����, �������� ���� � ��� ��������.
	std::string name = node.get_attribute(cResGenotypesName);
	std::string author = node.get_attribute(cResGenotypesAuthor);
	clan::Color aliveColor = clan::Color(clan::Colorf(node.get_attribute(cResGenotypesAliveColor)));
	clan::Color deadColor = clan::Color(clan::Colorf(node.get_attribute(cResGenotypesDeadColor)));
	std::string geneName = node.get_attribute(cResGenotypesGeneName);

	// ������ ���� ������ ��� ����� ������� � ��������� ��� � ������.
	std::shared_ptr<demi::GenotypesTree> newGenotypeTreeNode = std::make_shared<demi::GenotypesTree>();
	newGenotypeTreeNode->ancestor = ancestorTreeItem;
	ancestorTreeItem->derivatives.push_back(newGenotypeTreeNode);

	// ������ ��� �������������� (��� �������) �� ������ �� �����. ������� ��������� ������ ����� ���������� � ��������, ��������� ������������.
	demi::Gene gene(geneName, geneDatas.at(geneName));

	// ������ �������.
	std::shared_ptr<demi::Genotype> newGenotype = std::make_shared<demi::Genotype>(newGenotypeTreeNode, gene, name, author, aliveColor, deadColor);

	// ��������� ������� � ������.
	newGenotypeTreeNode->genotype = newGenotype;

	// ���������� �������� ���� ��� ��������� ���������.
	clan::DomNodeList& nodes = node.get_elements_by_tag_name(cResGenotypesType);
	for (int i = nodes.get_length() - 1; i >= 0; --i) {
		doLoadGenotypes(nodes.item(i).to_element(), newGenotypeTreeNode, geneDatas);
	}



	// ������ ����� �������.
}


void World::doLoadBinary(const std::string &filename)
{
	// ��������� �������� ����, ���� �� ����.
	if (clan::FileHelp::file_exists(filename + "b")) {

		// ���� � ����� ��������� � ������ ��� ��������� �������. � ��������� ����� ���������� ����� 'demib'.
		clan::DataBuffer data = clan::File::read_bytes(filename + "b");
		clan::MemoryDevice binFile(data);

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

		// ������ ������ ����� ����������.
		auto strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Species:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Species:", strSecMarker));

		// ��������� ���� ����������� ����� (�������� �����, ��������� � �����), ������ ��� ��������������.
		genotypesTree->clear();
		genotypesTree->loadFromFile(binFile);
			
		// �������� ��� ������ ������ ������� ����� ��� ����, ����� ��� ���������� ���������� ������������ ���� ���� �������.
		speciesDict_t speciesDict;
		genotypesTree->generateDict(speciesDict);

		// ������ ������ ������� �����.
		strSecMarker = binFile.read_string_nul();
		if (strSecMarker != "Dots:")
			throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinMarker), "Dots:", strSecMarker));

		// ��������� ����� �������� ����. ������ ����� ��������� ��� ���������� ����������.
		size_t dotsCount = worldSize.width * worldSize.height;
		for (size_t i = 0; i != dotsCount; ++i) {

			// ������� �����.
			demi::Dot &dot = arDots[i];

			// ���������� ��������� � �����.
			const size_t elemArraySize = sizeof(uint64_t) * elemCount;
			if (binFile.read(dot.res, elemArraySize) != elemArraySize)
				throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongBinCannotReadDots), i));

			// �������� � �����.
			dot.organism = doReadOrganism(binFile, speciesDict, getDotXYFromIndex(i));
		}
	}
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

	// ��������� ��������� � XML-���������.
	doSaveSettings(pResDoc);

	// ���������� ��������� �� ����.
	pResDoc->save(filename);

	// ���������� �������� ����.
	doSaveBinary(filename);

	// ��������� ���������� ������, ���� �� �������.
	if (prevRun) {
		threadRunFlag = true;
		threadEvent.notify_all();
	}
}


void World::doSaveSettings(std::shared_ptr<clan::XMLResourceDocument>& resDoc)
{
	// ��������� ������� ��� �������.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsAppearance).get_element();
	prop.set_attribute_int(cResGlobalsAppearanceLeft, appearanceTopLeft.x);
	prop.set_attribute_int(cResGlobalsAppearanceTop, appearanceTopLeft.y);
	prop.set_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// ���������� �����.
	prop = resDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, int(timeModel.year));
	prop.set_attribute_int(cResGlobalsTimeDay, int(timeModel.day));
	prop.set_attribute_int(cResGlobalsTimeSecond, int(timeModel.sec));

	// ������� ��������� ���������.
	for (size_t i = 0; i != elemCount; ++i) {

		// �������.
		prop = resDoc->get_resource(cResElementsSection + std::string("/") + arResNames[i]).get_element();

		// ��������� ��������.
		prop.set_attribute_bool(cResElementsVisibility, arResVisible[i]);
	}
}

void World::doSaveBinary(const std::string &filename)
{
	// ������ ����� � ������ ��� ��������� ����������.
	clan::MemoryDevice binFile;

	// ������� ������ �����.
	binFile.write_string_nul("Ver:1");

	// ������� ���������� ���������.
	binFile.write_string_nul("ElementsCount:" + clan::StringHelp::int_to_text(int(elemCount)));

	// ������� �������� ���������.
	for (size_t i = 0; i != elemCount; ++i)
		binFile.write_string_nul(arResNames[i]);

	// ���������� ������ ������ ����� ���������� � ���� �����.
	binFile.write_string_nul("Species:");

	// ���������� ���� ����� (�������� �����, ��������� � �����).
	genotypesTree->saveToFile(binFile);

	// ��� ����, ����� �� ������ ����� ������ ���������� ������ � ��� ��������� ��������, ������� ������� � ����� ������������ �������.
	speciesDict_t speciesDict;
	genotypesTree->generateDict(speciesDict);

	// ���������� ������ ������ ������� �����.
	binFile.write_string_nul("Dots:");

	// ���������� �����.
	size_t dotsCount = worldSize.width * worldSize.height;
	const size_t elemArraySize = sizeof(uint64_t) * elemCount;
	for (size_t i = 0; i != dotsCount; ++i) {
		// ������� �����.
		const demi::Dot &dot = arDots[i];

		// ���������� ��������� � �����.
		binFile.write(dot.res, elemArraySize);

		// �������� � �����.
		doWriteOrganism(binFile, speciesDict, dot.organism);
	}

	// ���������� ����� � ����. � ��������� ����� ���������� ����� 'demib'.
	clan::File::write_bytes(filename + "b", binFile.get_data());
}


void World::workerThread()
{
	// ������� ������� ������, ������������ ������.
	try
	{
		while (true)
		{
			// ��������������� �� ��������� ������, �������� ��� ������������� �������� (������������) �����.
			std::unique_lock<std::mutex> lock(threadMutex);
			threadEvent.wait(lock, [&]() { return threadRunFlag || threadExitFlag; });

			// ���� ���� ������� �� �����, ��������� ������.
			if (threadExitFlag)
				break;

			// ������������ �������� �����.
			lock.unlock();

			// ��������� �������� ������� ������, ���������� ����� � �������.
			calculateTPS();

			// ��������� ������ ������.
			makeTick();

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

	// ���� ��� ���������, ������� ��������� �� ������� ��������, ����� ����� �������� ��� �� ����������.
	if (!is_active)
		InitResMaxArray();
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
	}
	catch (...) {
		// ��� ������ ��������� ������ ������.
		globalWorld.loadModel(defaultFilename);
	}
}

// �������� �� SaveModel() ��� ��������. ������ ������ ���������.
void World::doWriteOrganism(clan::IODevice &binFile, speciesDict_t& speciesDict, demi::Organism* organism)
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

	// ���������� ����������� ���� ���������.
	organism->saveToFile(binFile);
}

demi::Organism* World::doReadOrganism(clan::IODevice &binFile, speciesDict_t& speciesDict, const clan::Point &center)
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
	return demi::Organism::createFromFile(binFile, center, species);
}

// �������������� ������ ���������� �� ������ ��������� ��������� � ������, ������������ ����� ��������.
void World::InitResMaxArray()
{
	// ������� ������ ��������.
	memset(arResMax, 0, sizeof(unsigned long long) * elemCount);

	// ���������� ��� ����� � ��������� ����������.
	demi::Dot *cur = arDots;										// ������ ����� �������.
	demi::Dot *last = cur + worldSize.width * worldSize.height;	// ����� ����� ��������� ����� �������.
	while (cur < last) {

		// ��������� ��������� - ��� ��� ������� �� ����� ��������.
		demi::Organism* curOrganism = cur->organism;
		if (curOrganism) {

			// ���� � �������� ����� ���� �������� � ��� ���� �������, ������� ���.
			if (curOrganism->needDesintegration()) {
				// �� ������� ����� �������� ������ ��� ���� � �����������.
				delete curOrganism;
			}
		}

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

// ��������� ����������� ������� � ���������� ������, ���� ��� ������ ����������.
bool World::activateMutation()
{
	// ���� ����� ����, ������ ���� ��������.
	return rnd_Mutation(generator) == 0;
}


// ��������� �������� ������� ������, ���������� ����� � �������.
void World::calculateTPS()
{
	// ��� ��-�������� � game_time.cpp
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

// ���������� ������ �������� �� ��������. ����� ��������� ��� ������� ������ �����, ���� ����� ��������������.
size_t World::findElemIndex(const std::string& elemName)
{
	// ���������� ��� �������� � ���������� ������.
	for (size_t i = 0; i != elemCount; ++i)
		if (arResNames[i] == elemName)
			return i;

	throw clan::Exception(clan::string_format(pSettings->LocaleStr(cWrongReaction), elemName));
}



