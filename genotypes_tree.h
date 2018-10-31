/* ===============================================================================
������������� �������� ������ ����.
����������� �������� ��������� � ����� ��� �������������� � �������������.
10 october 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once

#include <vector>

typedef std::vector<std::shared_ptr<demi::Species>> speciesDict_t;

//
// ����������� ������, � ������� - ��� ��������������.
//
namespace demi {

	class GenotypesTree {
	public:
		// ������������ ����, ���� ����.
		std::shared_ptr<GenotypesTree> ancestor = nullptr;

		// ������� ������� ���� ������.
		std::shared_ptr<demi::Genotype> genotype;

		// ������ ����������� �����.
		std::vector<std::shared_ptr<GenotypesTree>> derivatives;

		// ������ ����������� ����� �� �������� �������� � ���������.
		std::vector<std::shared_ptr<demi::Species>> species;

		// ���� ��� ��������� ������������� �������������������� ����������� ����������.
		static bool flagSpaciesChanged;

		// ������� �������.
		void clear() { derivatives.clear(); species.clear(); }

		// ������ ������� ��� ������������� ����� � ��������.
		void generateDict(speciesDict_t& dict);

		// ���������� ������ �� ����.
		void saveToFile(clan::IODevice& binFile);

		// ��������� ���� � �����.
		void loadFromFile(clan::IODevice& binFile);

		// ���������� ��������� �� ��� � ������ �������� ��������, �������� � ��������.
		const std::shared_ptr<demi::Species> breeding(const std::shared_ptr<demi::Species>& oldSpec);

		// ������ ���������� ����� �������� � �����.
		// ������ ���� ������� ���������� ���������� � ���������� ������, ���� �������. ���� ����, ���������� � ����� ��������� �����������.
		bool tryLockSpecies() { return !lockFlag.test_and_set(std::memory_order_acquire); }
		// ������������� ���������� �� ��� ���, ���� �� ������� ���������� ����������.
		void lockSpecies() { for (size_t i = 0; !tryLockSpecies(); ++i) if (i % 100 == 0) std::this_thread::yield(); }
		// ������� ����������.
		void unlockSpecies() { lockFlag.clear(std::memory_order_release); }
	private:
		// ��� ���������� �������������� ������� �� ������ ������� (���������� � �������������).
		std::atomic_flag lockFlag;
	};
};