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
#include "Earth.h"


const float cGeothermRadius = 12.0f;

// ��������� ��� ������ XML-����� ������
const std::string cResGlobalsEarthSize = "Globals/EarthSize";
const std::string cResGlobalsEarthSizeWidth = "width";
const std::string cResGlobalsEarthSizeHeightRatio = "heightRatio";

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
Earth globalEarth;

// =============================================================================
// ����� �� ������ ����������� � ������������ � ������� �������� � ��������� �� ����������.
// =============================================================================
Dot::Dot() 
{
	res = new float[globalEarth.getElemCount()];
	memset(res, 0, sizeof(float) * globalEarth.getElemCount());
}

Dot::~Dot()
{
	delete res;
}


void Dot::get_color(clan::Colorf &aValue) const
{
	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	//
	// ��������� ���� � ������� ��� �����-�����.
	aValue.set_alpha(solarEnergy > energy ? solarEnergy : energy);

	// ���� ���������� ������ ����� ������ ���������, ��������� ����� ����������� ������. 
	// ������� ����� ���� ���������� ���������������� ���� ��� ���� �����, �� ���� ����� �����. �����
	// � ������� �������� � �������� ��������� ��������.
	//

	// ����� (��������� �������) ������� ��� ������� ��������.
	float resBright = res[0] / globalEarth.arResMax[0];

	// ���� �������� �������.
	clan::Colorf &col = globalEarth.arResColors[0];

	// ���� �������, ����������� ��������������� ������.
	// ���������� �������� �������� 4 ����������, ������� ��� ����������� ������ ��� �������.
	//
	aValue.x = col.x;
	aValue.y = col.y;
	aValue.z = col.z;

	// � ����� �������� ������� ��������� ���������.
	//
	for (int i = 1; i < globalEarth.elemCount; ++i) {

		// ����� �������� �������.
		const float curResBright = res[i] / globalEarth.arResMax[i];

		// ���� ������� ����, ���������� ����.
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
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
// =============================================================================
Dot& LocalCoord::get_dot(float x, float y) const
{
	// ��� ��������.
	float ww = globalEarth.get_worldSize().width;
	float wh = globalEarth.get_worldSize().height;

	// �� ����������� ���������� ��������� � ������ ���� �� ������.
	x = roundf(x + center.x);
	if (x < 0)
		x += ww;
	else if (x >= ww)
		x -= ww;

	// �� ��������� ���� ������ ��������.
	y = roundf(y + center.y);
	if (y < 0) 
		y = 0;
	else if (y >= wh)
		y = wh - 1;

	//_ASSERT(x < globalEarth.get_worldWidth());
	//_ASSERT(y < globalEarth.get_worldHeight());

	return dots[int(x + y * ww)];
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
	LocalCoord coord(globalEarth.arDots, getPos(timeModel));

	// ��������� �� ���� ������
	//
	float lightRadius = globalEarth.getLightRadius();
	for (float x = -lightRadius; x <= lightRadius; x++)
		for (float y = -lightRadius; y <= lightRadius; y++) {

			// ����������� ���������� �� ������ ���������.
			float r = sqrt(x*x + y*y);

			// ���� ��� ������ ��������� �������, ������ �� ������, ����� ������� ������������, �������� ���������� � ����� �� 0 �� 1.
			coord.get_dot(x, y).solarEnergy = r < lightRadius ? (lightRadius - r) / lightRadius : 0.0f;
		}

}


clan::Pointf Solar::getPos(const DemiTime &timeModel)
{
	// ���������� ������� ��� ������ � ����������� �� �������.

	// ������� ����.
	const clan::Sizef worldSize = globalEarth.get_worldSize();

	// ������� �� ����������� ������� �� ������� �����.
	// ������ ��������� � ������� �� ����� ��������������� ��������� ���� �����.
	clan::Pointf result(worldSize.width * (1.0f - float(timeModel.sec - 1) / (cTicksInDay - 1)), 0.0f);

	// ������� �� ��������� ������� �� ��� ����.
	// ������ ������� ��������� �� ����� ������� �������� �� ������ � ������� � �������� �����������, 
	// �� ���� ����� ������� ������� �����������.
	
	// �������� ����, ��� ��������.
	const float halfYear = cDaysInYear / 2;

	// ������ ��������
	const float tropic = globalEarth.getTropicHeight();

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
Earth::Earth()
{
	// �������������� ��������� ��������� �����.
	srand((unsigned)::time(NULL));

	// �������� �����. �� �� �������� �� ��������� �����.
	thread = std::thread(&Earth::worker_thread, this);
}



Earth::~Earth()
{
	// �������� ������ ������, ���� �� ���� ������, �� ��� �� ����� ����������� ���������.
	//
	std::unique_lock<std::mutex> lock(thread_mutex);
	thread_exit_flag = true;
	lock.unlock();
	threadMainToWorkerEvent.notify_all();
	thread.join();

	delete[] arDots;
	delete[] arDotsCopy;
	delete[] arResColors;
	delete[] arResMax;
	delete[] arResNames;
	delete[] arResVolatility;
	delete[] arEnergy;
}


void Earth::MakeTick()
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



void Earth::FillRectResource(int resId, float amount, const clan::Rectf &rect)
{
	// ����� ������������� �������� �� ��������� ������������� ������� � ��������� ����������.
	//
	// �������������� ��������, ���� ���������.
	//
	if (arResMax[resId] < amount)
		arResMax[resId] = amount;

	for (float x = rect.left; x < rect.right; ++x)
		for (float y = rect.top; y < rect.bottom; ++y)
			arDots[int(x + y * worldSize.width)].res[resId] = amount;
}


void Earth::Diffusion()
{
	// �������� ��������.
	//
	// ������� �������, �� ������� �������� ������.
	Dot *cur = arDots;	// �������� ����� ��� �������� �������� - ������ ����� �������.
	Dot *last = cur + int(worldSize.width * worldSize.height);	// ����� ����� ��������� ����� �������.
	Dot *dest;			// �������� �����

	while (true) {

		// ��������� �����.
		int random = rand();
		int rnd = random % 7; // �� 0 �� 7, ����������� ��������
		int rndRes = random % elemCount; // ��������� �������.

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
		//
		if (dest < arDots)
			dest = last - (arDots - dest);
		else if (dest >= last)
			dest = arDots + (dest - last);

		// ���������� ������� �������� �� �������� ����� � �������� �������� ��������� �������.
		//
		Dot &fromDot = *cur;
		Dot &toDot = *dest;
		const float amount = fromDot.res[rndRes] * arResVolatility[rndRes];
		fromDot.res[rndRes] -= amount;
		toDot.res[rndRes] += amount;

		// ������� ���������.
		//
		if (arResMax[rndRes] < toDot.res[rndRes])
			arResMax[rndRes] = toDot.res[rndRes];
	}
}


void Earth::AddGeothermal(int i, const clan::Pointf &coord)
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
				geothermalCoord.get_dot(xp, yp).energy = (cGeothermRadius - r) / cGeothermRadius;
		}
}


void Earth::LoadModel(const std::string &filename)
{
	// ��������� ������ �� xml-�����
	//
	// ���� ����� ��� �������, ������� �� ������.
	{
		std::unique_lock<std::mutex> lock(thread_mutex);
		if (thread_run_flag)
			throw clan::Exception("Need to stop the evolution prior to load!");
	}

	// ������� XML ����.
	auto resDoc = std::make_shared<clan::XMLResourceDocument>(filename);

	// �������������� ������� ����.
	//
	const clan::DomElement &prop = resDoc->get_resource(cResGlobalsEarthSize).get_element();

	// ������ ����.
	worldSize.width = float(prop.get_attribute_int(cResGlobalsEarthSizeWidth));

	// ������ ����
	worldSize.height = round(worldSize.width * prop.get_attribute_float(cResGlobalsEarthSizeHeightRatio));

	if (worldSize.width <= 0 || worldSize.width > 30000 || worldSize.height <= 0 || worldSize.height > 30000)
		throw clan::Exception("Invalid world size (width=" + clan::StringHelp::int_to_text(int(worldSize.width)) 
			+ ", height=" + clan::StringHelp::int_to_text(int(worldSize.height)) + ")");

	// ������ ���������� ����� � ������ �������� ������� �� ������� ����.
	lightRadius = round(0.9f * worldSize.height / 2);
	tropicHeight = round(worldSize.height / 5);

	// �������� ��������.
	const std::vector<std::string>& names = resDoc->get_resource_names_of_type(cResElementsType, cResElementsSection);

	// ���������� ��������.
	elemCount = int(names.size());

	// ��������� �������.
	const std::vector<std::string>& energy = resDoc->get_resource_names_of_type(cResEnergyType, cResEnergySection);

	// ���������� ����������.
	energyCount = int(energy.size());

	// ������� ������ ��� �������.
	//
	arDots = new Dot[int(worldSize.width * worldSize.height)];		// ����� �����������.
	arResColors = new clan::Colorf[elemCount];
	arResMax = new float[elemCount];
	arResNames = new std::string[elemCount];
	arResVolatility = new float[elemCount];
	arEnergy = new Geothermal[energyCount];			// ������������� ���������.

	// �������������� ������ ���������� ���������, ��-��������� ������� �� ����.
	//
	for (int i = 0; i < elemCount; ++i)
		arResMax[i] = 1.0f;


	//
	// ��������� �������� ���������.
	//
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
		//
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
	//
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
}


void Earth::SaveModel(const std::string &filename)
{
	// ��������� ������ � xml-����
	//
	// XML-���� �� ���������, ��� ��� �� �� �������� �������.
}


void Earth::worker_thread()
{
	// ������� ������� ������, ������������ ������.
	//
	try
	{
		while (true)
		{
			// ��������������� �� ��������� ������, �������� ��� ������������� �������� �����.
			std::unique_lock<std::mutex> lock(thread_mutex);
			threadMainToWorkerEvent.wait(lock, [&]() { return thread_run_flag || thread_exit_flag; });

			// ���� ���� ������� �� �����, ��������� ������.
			if (thread_exit_flag)
				break;

			// ���������� ���� ������� ����������.
			//thread_complete_flag = false;

			// ������������ �������� �����.
			lock.unlock();

			// ��������� ������ ������.
			MakeTick();

			// ��������� �������� ����� ��� ���������� ����������.
			lock.lock();

			// �������� ��������� ������ � �����, ������������ ��� �����������.
			timeBackup = timeModel;

			// �������� � ������� ����������.
			//thread_complete_flag = true;
			//threadWorkerToMainEvent.notify_all();


			//throw clan::Exception("Bang!");	// <--- Use this to test the application handles exceptions in threads
			// ��� �������� ����� �������������� ��� ���������� ����� {}
		}
	}
	catch (clan::Exception &)
	{
		// ������� �� ��������� ���������� ������. ����� ������ ������� �����.
		std::unique_lock<std::mutex> lock(thread_mutex);
		thread_crashed_flag = true;
	}
}

void Earth::RunEvolution(bool is_active)
{
	// ���������������� ��� ���������� ������ ������
	//
	std::unique_lock<std::mutex> lock(thread_mutex);
	thread_run_flag = is_active;
	threadMainToWorkerEvent.notify_all();
}


//Dot * Earth::getCopyDotsArray()
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
