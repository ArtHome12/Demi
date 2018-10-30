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

	// ���������� ���������� �������� � ���������� ��������, �� ������� ��, ��� ����� ���� � ��������� � �������.
	uint64_t getElemAmount(size_t index) const { return res[index]; }
	void setElemAmount(size_t index, uint64_t amount) { res[index] = amount; }

	// ����������� � ��������� ���������� ��������.
	void incElemAmount(size_t index, uint64_t amount) { res[index] += amount; }
	void decElemAmount(size_t index, uint64_t amount) { res[index] -= amount; }

	// ���������� ���������� �������� � ���������.
	float getElemAmountPercent(size_t index) const;

	// ��������� � ��� ������� - ������ ��� �������� ��� ��������� � ������������� ������� (��� ��������� ����������).
	uint64_t *res;

	// �����������, ������ ����� ��������.
	Dot();
	Dot(const Dot &obj) = delete;
	~Dot();
	void operator=(const Dot &obj) = delete;

	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	void getColor(clan::Color &aValue) const;

	// ������ ����������, ����������� � �����.
	std::vector<std::shared_ptr<demi::GenericCell>> cells;

	// ��������� �� �������� � ����� ��� nullptr. � ����� ��������� ���� �� ������ ������ ���� � ������� �����. ��� �����������,
	// ����� �� ������ ��������� ����� ������.
	demi::Organism* organism = nullptr;

private:
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
	clan::Point getGlobalPoint() const { return center; }
	clan::Point getGlobalPoint(const clan::Point &localPoint) const { return center + localPoint; }

private:
	// ������� ���� �� ������ ��������, ��� ����������.
	size_t worldWidth;
	size_t worldHeight;

	// ����� ��������� ���������, ���������� � ���������� �����������.
	clan::Point center;
};


