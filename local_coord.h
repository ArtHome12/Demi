/* ===============================================================================
������������� �������� ������ ����.
������ ��� ������������� ������� ���������.
03 january 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

namespace demi
{
	class GenericCell;
	class Organism;
}

//
// ����� �� ������ ����������� �� ������� �������� � ��������� �� ����������.
//
class Dot {
public:

	// ��������� ������� � �����.
	float getSolarEnergy() const { return  solarEnergy; }
	void setSolarEnergy(float newVal) { solarEnergy = newVal; }

	// ������������� ������� � �����.
	float getGeothermalEnergy() const { return geothermalEnergy; }
	void setGeothermalEnergy(float newVal) { geothermalEnergy = newVal; }

	// ���������� ���������� �������� � ���������� ��������.
	unsigned long long getElemAmount(int index) const { return res[index]; }
	void setElementAmount(int index, unsigned long long amount) { res[index] = amount; }

	// ���������� ���������� �������� � ���������.
	float getElemAmountPercent(int index) const;

	// ��������� � ��� ������� - ������ ��� �������� ��� ��������� � ������������� ������� (��� ��������� ����������).
	unsigned long long *res;

	// �����������, ������ ����� ��������.
	Dot();
	Dot(const Dot &obj);
	~Dot();
	Dot& operator=(const Dot &obj);

	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	void get_color(clan::Color &aValue) const;

	// ������ ����������, ����������� � �����.
	std::vector<std::shared_ptr<demi::GenericCell>> cells;

	// ��������� �� �������� � ����� ��� nullptr. � ����� ��������� ���� �� ������ ������ ���� � ������� �����. ��� �����������,
	// ����� �� ������ ��������� ����� ������.
	demi::Organism* organism = nullptr;

private:
	// ����������� � ������ ��� ����� ����� ������������ �����������. ����� �� ����������� ������ ��-��� res.
	bool isCopy;

	float solarEnergy = 0.0f;
	float geothermalEnergy = 0.0f;
};


//
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
//
class LocalCoord {
public:
	LocalCoord(const clan::Point &coord);

	// ���������� ����� � ��������� ����������.
	Dot& get_dot(int x, int y) const;
	Dot& get_dot(const clan::Point &coord) const { return get_dot(coord.x, coord.y); }

	// ���������� ����� ����� � ���������� �����������.
	clan::Point getGlobalPoint() { return center; }
	clan::Point getGlobalPoint(const clan::Point &localPoint) { return center + localPoint; }

private:
	// ������ � ������� �����������.
	Dot *dots;

	// ������� ���� �� ������ ��������, ��� ����������.
	int worldWidth;
	int worldHeight;

	// ����� ��������� ���������, ���������� � ���������� �����������.
	clan::Point center;
};


