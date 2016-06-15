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
#include "string_resources.h"
#include "world.h"


const float cGeothermRadius = 12.0f;

// ��������� ��� ������ XML-����� ������
const std::string cResGlobalsWorldSize = "Globals/WorldSize";
const std::string cResGlobalsWorldSizeWidth = "width";
const std::string cResGlobalsWorldSizeHeightRatio = "heightRatio";

const std::string cResGlobalsAppearance = "Globals/Appearance";
const std::string cResGlobalsAppearanceTop = "top";
const std::string cResGlobalsAppearanceLeft = "left";
const std::string cResGlobalsAppearanceScale = "scale";

const std::string cResGlobalsTime = "Globals/Time";
const std::string cResGlobalsTimeYear = "year";
const std::string cResGlobalsTimeDay = "day";
const std::string cResGlobalsTimeSecond = "second";

const std::string cResElementsSection = "Elements";
const std::string cResElementsType = "Element";

const std::string cResEnergySection = "Energy";
const std::string cResEnergyType = "Geothermal";

const std::string cResOrganismsSection = "Organisms";
const std::string cResOrganismsType = "Organism";

const std::string cResAreaRect = "rect";	// ��� ��� ������������� ������� ���������� ������������� �������/����������.
const std::string cResAreaPoint = "point";	// ��� ��� �����.

const std::string cElementsResColor = "color";
const std::string cElementsResVolatility = "volatility";

const std::string cSolar = "Solar";			// ����������� ��������� �������.
const std::string cEnergy = "Geothermal";			// ����������� �������.


// ���������� ������ - ������� ���.
World globalWorld;

// =============================================================================
// ����� �� ������ ����������� � ������������ � ������� �������� � ��������� �� ����������.
// =============================================================================
Dot::Dot() 
{
	// �������� ������ ��� ���������� �������� ���� ��������� � ������������� �������.
	int elemCount = globalWorld.getElemCount() + 2;
	res = new float[elemCount];
	memset(res, 0, sizeof(float) * elemCount);
}

Dot::~Dot()
{
	delete res;
}

int Dot::getSizeInMemory() 
{
	// ����� ������ � ������ ��� ������ - ������ ���������� �������� ���� ��������� � ������������� �������.
	return (globalWorld.getElemCount() + 2) * sizeof(float);
}

void Dot::get_color(clan::Colorf &aValue) const
{
	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	//
	// ��������� ���� � ������� ��� �����-�����.
	aValue.set_alpha(clan::max<float, float>(getSolarEnergy(), getGeothermalEnergy()));

	// ���� ���������� ������ ����� ������ ���������, ��������� ����� ����������� ������. 
	// ������� ����� ���� ���������� ���������������� ���� ��� ���� �����, �� ���� ����� �����. �����
	// � ������� �������� � �������� ��������� ��������.
	//

	// ����� (��������� �������) ������� ��� ������� ��������.
	float resBright = res[2] / globalWorld.arResMax[0];

	// ���� �������� �������.
	clan::Colorf &col = globalWorld.arResColors[0];

	// ���� �������, ����������� ��������������� ������.
	// ���������� �������� �������� 4 ����������, ������� ��� ����������� ������ ��� �������.
	//
	aValue.x = col.x;
	aValue.y = col.y;
	aValue.z = col.z;

	// � ����� �������� ������� ��������� ���������.
	//
	for (int i = 1; i < globalWorld.elemCount; ++i) {

		// ����� �������� �������.
		const float curResBright = res[i + 2] / globalWorld.arResMax[i];

		// ���� ������� ����, ���������� ����.
		//
		if (resBright < curResBright) {
			resBright = curResBright;
			clan::Colorf &col = globalWorld.arResColors[i];
			aValue.x = col.x;
			aValue.y = col.y;
			aValue.z = col.z;
		}
	}
}

// =============================================================================
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
// =============================================================================
LocalCoord::LocalCoord(Dot *arDots, const clan::Pointf &coord) : dots(arDots),
	center(coord),
	worldWidth(globalWorld.get_worldSize().width),
	worldHeight(globalWorld.get_worldSize().height) 
{
};

Dot& LocalCoord::get_dot(float x, float y) const
{
	// �� ����������� ���������� ��������� � ������ ���� �� ������.
	x = roundf(x + center.x);
	if (x < 0)
		x += worldWidth;
	else if (x >= worldWidth)
		x -= worldWidth;

	// �� ��������� ���� ������ ��������.
	y = roundf(y + center.y);
	if (y < 0) 
		y = 0;
	else if (y >= worldHeight)
		y = worldHeight - 1;

	//_ASSERT(x < globalWorld.get_worldWidth());
	//_ASSERT(y < globalWorld.get_worldHeight());

	return dots[int(x + y * worldWidth)];
}

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
	LocalCoord coord(globalWorld.arDots, getPos(timeModel));

	// ��������� �� ���� ������
	//
	float lightRadius = globalWorld.getLightRadius();
	for (float x = -lightRadius; x <= lightRadius; x++)
		for (float y = -lightRadius; y <= lightRadius; y++) {

			// ����������� ���������� �� ������ ���������.
			float r = sqrt(x*x + y*y);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			coord.get_dot(x, y).setSolarEnergy(r < lightRadius ? (lightRadius - r) / lightRadius : 0.0f);
		}

}


clan::Pointf Solar::getPos(const DemiTime &timeModel)
{
	// ���������� ������� ��� ������ � ����������� �� �������.

	// ������� ����.
	const clan::Sizef worldSize = globalWorld.get_worldSize();

	// ������� �� ����������� ������� �� ������� �����.
	// ������ ��������� � ������� �� ����� ��������������� ��������� ���� �����.
	clan::Pointf result(worldSize.width * (1.0f - float(timeModel.sec - 1) / (cTicksInDay - 1)), 0.0f);

	// ������� �� ��������� ������� �� ��� ����.
	// ������ ������� ��������� �� ����� ������� �������� �� ������ � ������� � �������� �����������, 
	// �� ���� ����� ������� ������� �����������.
	
	// �������� ����, ��� ��������.
	const float halfYear = cDaysInYear / 2;

	// ������ ��������
	const float tropic = globalWorld.getTropicHeight();

	// ����� ��� ������ �������� ����, ���� �� �������� �������� ����, � ����� ������ - ����������.
	//
	if (timeModel.day < halfYear)
		result.y = (worldSize.height - tropic) / 2 + timeModel.day * tropic / halfYear;
	else
		result.y = (worldSize.height + tropic) / 2 - (timeModel.day - halfYear) * tropic / halfYear;

	return result;
}



// =============================================================================
// ������ �����������, ��������� ������ �����.
// =============================================================================
World::World()
{
	// �������������� ��������� ��������� �����.
	srand((unsigned)::time(NULL));

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
	delete[] arDotsCopy;
	delete[] arResColors;
	delete[] arResMax;
	delete[] arResNames;
	delete[] arResVolatility;
	delete[] arEnergy;
}


void World::MakeTick()
{
	// �������� ���������.
	//
	// ��������� ����� ����.
	timeModel.MakeTick();

	// �������� ��� ��������� ��������.
	solar.Shine(timeModel);

	// ������������� �������� ��-�� ��������.
	Diffusion();
}



void World::FillRectResource(int resId, float amount, const clan::Rectf &rect)
{
	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	//
	// �������������� ��������, ���� ���������.
	//
	if (arResMax[resId] < amount)
		arResMax[resId] = amount;

	for (float x = rect.left; x < rect.right; ++x)
		for (float y = rect.top; y < rect.bottom; ++y)
			arDots[int(x + y * worldSize.width)].res[resId + 2] = amount;
}


void World::Diffusion()
{
	// �������� ��������.

	// ������� �������, �� ������� �������� ������.
	Dot *cur = arDots;											// �������� ����� ��� �������� �������� - ������ ����� �������.
	Dot *last = cur + int(worldSize.width * worldSize.height);	// ����� ����� ��������� ����� �������.
	Dot *dest;													// �������� �����

	while (true) {

		// ��������� �����.
		int random = rand();
		int rnd = random % 7;					// �� 0 �� 7, ����������� ��������
		int rndResA = random % elemCount;		// ��������� �������.
		int rndResB = rndResA + 2;				// ���� ���������� ������ ��� ��������� � ������������� �������.

		// ���������� �������� ���������� ��������� ��������� ������, ����� ��������� ���������� ����� ��������� ������ � ������������.
		cur += rand() % 12;

		// ���� ���������� ��� �����, �����������.
		if (cur >= last)
			break;

		// ���������� �������� ���������� - � ���� �� ������ �� ��������.
		switch (rnd) {
		case 1 :
			dest = cur + 1;					// �� ������.
			break;
		case 2 :
			dest = cur + 1 + int(worldSize.width);	// �� ���-������.
			break;
		case 3 :
			dest = cur + int(worldSize.width);		// �� ��.
			break;
		case 4 :
			dest = cur - 1 + int(worldSize.width);	// �� ���-�����.
			break;
		case 5 :
			dest = cur - 1;					// �� �����.
			break;
		case 6 :
			dest = cur - 1 - int(worldSize.width);	// �� ������-�����.
			break;
		default:
			dest = cur - int(worldSize.width);		// �� �����.
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
		const float amount = fromDot.res[rndResB] * arResVolatility[rndResA];
		fromDot.res[rndResB] -= amount;
		toDot.res[rndResB] += amount;

		// ������� ���������.
		//
		if (arResMax[rndResA] < toDot.res[rndResB])
			arResMax[rndResA] = toDot.res[rndResB];
	}
}


void World::AddGeothermal(int i, const clan::Pointf &coord)
{
	// ����� �������������� ���������� ������������� �������.
	//

	arEnergy[i].coord = coord;

	// ��� ��� ������������� ��������� �� ������ ������ ������� �������������, ����� �� ������� ������������� �������.
	//

	// ��������� ������� ���������.
	LocalCoord geothermalCoord(arDots, coord);

	for (float xp = -cGeothermRadius; xp <= cGeothermRadius; xp++)
		for (float yp = -cGeothermRadius; yp <= cGeothermRadius; yp++) {

			// ����������� ���������� �� ������ ���������.
			float r = sqrt(xp*xp + yp*yp);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			if (r < cGeothermRadius) 
				geothermalCoord.get_dot(xp, yp).setGeothermalEnergy((cGeothermRadius - r) / cGeothermRadius);
		}
}


void World::LoadModel(const std::string &filename)
{
	// ��������� ������ �� xml-�����

	// ���� ����� ��� �������, ������� �� ������.
	{
		std::unique_lock<std::mutex> lock(threadMutex);
		if (threadRunFlag)
			throw clan::Exception(globalStr.getStr("WorldLoadModelErrorNeedToStop"));
	}

	// ������� XML ����.
	auto resDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// �������������� ������� ����.
	clan::DomElement &prop = resDoc->get_resource(cResGlobalsWorldSize).get_element();

	// ������ ����.
	worldSize.width = float(prop.get_attribute_int(cResGlobalsWorldSizeWidth, 1000));

	// ������ ����
	worldSize.height = round(worldSize.width * prop.get_attribute_float(cResGlobalsWorldSizeHeightRatio, 1.0f));

	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception("Invalid world size (width=" + clan::StringHelp::int_to_text(int(worldSize.width)) 
			+ ", height=" + clan::StringHelp::int_to_text(int(worldSize.height)) + ")");

	// ������ ���������� ����� � ������ �������� ������� �� ������� ����.
	lightRadius = round(0.9f * worldSize.height / 2);
	tropicHeight = round(worldSize.height / 5);

	// �������������� ������� ��� �������.
	prop = resDoc->get_resource(cResGlobalsAppearance).get_element();
	appearanceTopLeft.x = float(prop.get_attribute_int(cResGlobalsAppearanceLeft, int(appearanceTopLeft.x)));
	appearanceTopLeft.y = float(prop.get_attribute_int(cResGlobalsAppearanceTop, int(appearanceTopLeft.y)));
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
	arDots = new Dot[int(worldSize.width * worldSize.height)];	// ����� �����������.
	arResColors = new clan::Colorf[elemCount];					// ����� ���������.
	arResMax = new float[elemCount];							// ������������ ������������ ��������� � ����� �����.
	arResNames = new std::string[elemCount];					// �������� ���������.
	arResVolatility = new float[elemCount];						// ��������� ���������.
	arEnergy = new Geothermal[energyCount];						// ������������� ���������.

	// �������������� ������ ���������� ���������, ��-��������� ������� �� ����.
	for (int i = 0; i < elemCount; ++i)
		arResMax[i] = 1.0f;


	// ��������� �������� ���������.
	for (int i = 0; i < elemCount; ++i) {

		// �������.
		clan::XMLResourceNode &res = resDoc->get_resource(names[i]);

		// �������� ��������.
		arResNames[i] = res.get_name();

		// �������� ��������.
		clan::DomElement &prop = res.get_element();

		// ���� ��� �����������.
		arResColors[i] = clan::Colorf(prop.get_attribute(cElementsResColor));

		// ���������, �������� �� 0 �� 1.
		arResVolatility[i] = prop.get_attribute_float(cElementsResVolatility);

		// ������� ��� ���������� ��������.

		// ���� ������������� ������� � ����� ������������� �������.
		clan::DomNodeList nodes = prop.get_elements_by_tag_name(cResAreaRect);
		for (int j = nodes.get_length() - 1; j >= 0; --j) {

			// ��������� ������� � ����������� � ������������� �������.
			clan::DomElement &rectItem = nodes.item(j).to_element();

			float r = float(rectItem.get_attribute_int("right"));
			if (r == 0)
				r = worldSize.width;

			float b = float(rectItem.get_attribute_int("bottom"));
			if (b == 0)
				b = worldSize.height;

			// �������.
			clan::Rectf rect(float(rectItem.get_attribute_int("left")), float(rectItem.get_attribute_int("top")), r, b);

			// ��������� �����.
			FillRectResource(i, rectItem.get_attribute_float("amount"), rect);
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
			AddGeothermal(i, clan::Pointf(float(pointItem.get_attribute_int("x")), float(pointItem.get_attribute_int("y"))));
		}
	}

	// ��������� �������� ����, ���� �� ����.
	if (clan::FileHelp::file_exists(filename + "b")) {

		clan::File binFile(filename + "b",				// � ��������� ����� ���������� ����� 'demib'.
			clan::File::OpenMode::open_existing,
			clan::File::AccessFlags::access_read,
			clan::File::ShareFlags::share_read);

		// ������ �����.
		const std::string &strVer = binFile.read_string_nul();
		if (strVer != "Ver:1")
			throw clan::Exception("Incorrect version binary file: need Ver:1, really " + strVer);

		// ���������� ���������.
		const std::string &strElemCount = binFile.read_string_nul();
		const std::string &strElemCountAwait = "ElementsCount:" + clan::StringHelp::int_to_text(elemCount);
		if (strElemCount != strElemCountAwait)
			throw clan::Exception("Incorrect version binary file: need " + strElemCountAwait + ", really " + strElemCount);


		// �������� ���������.
		for (int i = 0; i < elemCount; i++) {
			const std::string &elemName = binFile.read_string_nul();
			if (elemName != arResNames[i])
				throw clan::Exception("Incorrect version binary file: need " + arResNames[i] + ", really " + elemName);
		}

		// ������ ������ ������� �����.
		const std::string &strDotsMarker = binFile.read_string_nul();
		if (strDotsMarker != "Dots:")
			throw clan::Exception("Incorrect version binary file: need Dots:, really " + strDotsMarker);

		// ��������� �����.
		int dotsCount = int(worldSize.width * worldSize.height);
		int dotSize = Dot::getSizeInMemory();
		for (int i = 0; i < dotsCount; i++) {
			if (binFile.read(arDots[i].res, dotSize) != dotSize)
				throw clan::Exception("Incorrect version binary file: cannot read dot #" + clan::StringHelp::int_to_text(i));
		}

		// ������ ������ ������� ������������.
		const std::string &strResMaxMarker = binFile.read_string_nul();
		if (strResMaxMarker != "ResMax:")
			throw clan::Exception("Incorrect version binary file: need ResMax:, really " + strDotsMarker);

		// ��������� ������������ ������������.
		dotSize = elemCount * sizeof(float);
		if (binFile.read(arResMax, dotSize) != dotSize)
			throw clan::Exception("Incorrect version binary file: cannot read ResMax");

		// ������� ����.
		binFile.close();
	}
}


void World::SaveModel(const std::string &filename)
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
	prop.set_attribute_int(cResGlobalsAppearanceLeft, int(appearanceTopLeft.x));
	prop.set_attribute_int(cResGlobalsAppearanceTop, int(appearanceTopLeft.y));
	prop.set_attribute_float(cResGlobalsAppearanceScale, appearanceScale);

	// ���������� �����.
	prop = pResDoc->get_resource(cResGlobalsTime).get_element();
	prop.set_attribute_int(cResGlobalsTimeYear, timeModel.year);
	prop.set_attribute_int(cResGlobalsTimeDay, timeModel.day);
	prop.set_attribute_int(cResGlobalsTimeSecond, timeModel.sec);

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
	for (int i = 0; i < elemCount; i++)
		binFile.write_string_nul(arResNames[i]);

	// ���������� ������ ������ ������� �����.
	binFile.write_string_nul("Dots:");

	// ���������� �����. ����� ��� �� ����������, ��� ��� ������ �� �������.
	int dotsCount = int(worldSize.width * worldSize.height);
	int dotSize = Dot::getSizeInMemory();
	for (int i = 0; i < dotsCount; i++)
		binFile.write(arDots[i].res, dotSize);

	// ���������� ������ ������ ������� ����������.
	binFile.write_string_nul("ResMax:");

	// ���������� ��������� ������������ ��������.
	binFile.write(arResMax, elemCount * sizeof(float));

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
			MakeTick();

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

void World::RunEvolution(bool is_active)
{
	// ���������������� ��� ���������� ������ ������
	std::unique_lock<std::mutex> lock(threadMutex);
	threadRunFlag = is_active;
	threadEvent.notify_all();
}

// �������� ������ ������.
void World::ResetModel(const std::string &modelFilename, const std::string &defaultFilename)
{
	// ������� �������� ����.
	if (clan::FileHelp::file_exists(modelFilename + "b"))
		clan::FileHelp::delete_file(modelFilename + "b");

	// ���������� ��������� ������.
	try {
		globalWorld.LoadModel(modelFilename);
		timeModel = DemiTime();
		timeBackup = timeModel;
	}
	catch (...) {
		// ��� ������ ��������� ������ ������.
		globalWorld.LoadModel(defaultFilename);
	}
}

//Dot * World::getCopyDotsArray()
//{
//	// ���������������� ������ ������, �������� ��� � ����� � ����� ��������� ������ ������.
//	//
//
//	// ���� ��������� ����� �������, ���� ��������� �� ��������� ����������.
//	if (thread_run_flag) {
//		// ��������������� �� ��������� ����� ����������, �������� ��� ������������� ��������� �����.
//		std::unique_lock<std::mutex> lock(thread_mutex);
//		threadWorkerToMainEvent.wait(lock, [&]() { return thread_complete_flag; });
//
//		// ���������� ���������.
//		return arDotsCopy;
//
//		// ������������ ��������� ����� ��� ������ �� ����� {}.
//	}
//	else {
//		// ���� ��������� ����� �� ��������, ����� ����� ���������� ���������.
//		return arDotsCopy;
//	}
//
//	return arDotsCopy;
//}
