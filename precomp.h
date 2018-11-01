/* ===============================================================================
	������������� �������� ������ ����.
	����������� ����, ���� ��������� ���������� �������.
	24 august 2013.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/sound.h>
#ifndef __APPLE__
#include <ClanLib/d3d.h>
#endif
#include <ClanLib/gl.h>
#include <ClanLib/ui.h>
#include <clanlib/xml.h>


#include <cmath>
#include <thread>
#include <condition_variable>
#include <random>

// ����������� ����� ����� � ������ � �������� �� ��������.
template<typename T> std::string IntToStrWithDigitPlaces(T num);

// ����������� ����� ����� � ������ � �������� �� ��������.
template<typename T> std::string IntToStrWithDigitPlaces(T num)
{
	std::string retVal = std::to_string(num);

	// ������� ������� ��� ���������� ��������.
	int i = int(retVal.length()) - 3;
	while (i > 0) {
		retVal.insert(size_t(i), " ");
		i -= 3;
	}

	return retVal;
}

// ������ ����� ���������.
namespace demi {

	class Species;
}
typedef std::vector<std::shared_ptr<demi::Species>> speciesDict_t;

// ��� �������� ���� � ��� ����������� ��������� ��������.
typedef uint16_t geneValues_t;
#define geneValues_t_MAX UINT16_MAX


// ��� ��� �������� ������� ����������� ������� � ���������.
typedef uint8_t organismAmount_t;
typedef std::vector<organismAmount_t> organismAmounts_t;

// ����������� ��������� ����������� (���������) �������.
const uint16_t cMaxVitality = UINT16_MAX - UINT8_MAX;

