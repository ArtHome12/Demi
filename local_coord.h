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
	float getSolarEnergy() const { return res[0]; }
	void setSolarEnergy(float newVal) { res[0] = newVal; }

	// ������������� ������� � �����.
	float getGeothermalEnergy() const { return res[1]; }
	void setGeothermalEnergy(float newVal) { res[1] = newVal; }

	// ���������� ���������� �������� � ���������� ��������.
	float getElemAmount(int index) const { return res[index + 2]; }
	void setElementAmount(int index, float amount) { res[index + 2] = amount; }

	// ���������� ���������� �������� � ���������.
	float getElemAmountPercent(int index) const;


	// ����� ������ � ������ ��� ������ - ������ ���������� �������� ���� ��������� � ������������� �������.
	static int getSizeInMemory();

	// ��������� � ��� ������� - ������ ��� �������� ��� ��������� � ������������� ������� (��� ��������� ����������).
	float *res;

	// �����������, ������ ����� ��������.
	Dot();
	~Dot();

	// ���������� ���� ��� ����� �� ������ ��������� ��������, �������� �� ������ ��� �����������.
	void get_color(clan::Colorf &aValue) const;

	// ������ ����������, ����������� � �����.
	std::vector<std::shared_ptr<demi::GenericCell>> cells;

	// ��������� �� �������� � ����� ��� nullptr. � ����� ��������� ���� �� ������ ������ ���� � ������� �����. ��� �����������,
	// ����� �� ������ ��������� ����� ������.
	demi::Organism* organism = nullptr;
};


//
// ��������� ���������� - ����� ������ � ����� 0, 0 � ����� ���������� ������������� ����������.
//
class LocalCoord {
public:
	LocalCoord(const clan::Pointf &coord);

	Dot& get_dot(float x, float y) const;

private:
	// ������ � ������� �����������.
	Dot *dots;

	// ������� ���� �� ������ ��������, ��� ����������.
	float worldWidth;
	float worldHeight;

	// ����� ��������� ���������, ���������� � ���������� �����������.
	clan::Pointf center;
};


