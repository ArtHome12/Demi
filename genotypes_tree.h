/* ===============================================================================
Моделирование эволюции живого мира.
Древовидная иерархия генотипов и видов для взаимодействия с пользователем.
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
// Древовидный список, в вершине - вид протоорганизма.
//
namespace demi {

	class GenotypesTree {
	public:
		// Родительский узел, если есть.
		std::shared_ptr<GenotypesTree> ancestor = nullptr;

		// Текущий генотип узла дерева.
		std::shared_ptr<demi::Genotype> genotype;

		// Список производных узлов.
		std::vector<std::shared_ptr<GenotypesTree>> derivatives;

		// Список производных видов от текущего генотипа с индексами.
		std::vector<std::shared_ptr<demi::Species>> species;

		// Флаг для индикации необходимости переинициализировать отображение организмов.
		static bool flagSpaciesChanged;

		// Очищает векторы.
		void clear() { derivatives.clear(); species.clear(); }

		// Создаёт словарь для сопоставления видов и индексов.
		void generateDict(speciesDict_t& dict);

		// Записывает дерево на диск.
		void saveToFile(clan::IODevice& binFile);

		// Счтиывает себя с диска.
		void loadFromFile(clan::IODevice& binFile);

		// Возвращает указатель на вид в рамках текущего генотипа, возможно с мутацией.
		const std::shared_ptr<demi::Species> breeding(const std::shared_ptr<demi::Species>& oldSpec);

		// Должны вызываться перед доступом к видам.
		// Делает одну попытку установить блокировку и возвращает истину, если удалось. Если ложь, обращаться к полям организма небезопасно.
		bool tryLockSpecies() { return !lockFlag.test_and_set(std::memory_order_acquire); }
		// Останавливает выполнение до тех пор, пока не удастся установить блокировку.
		void lockSpecies() { for (size_t i = 0; !tryLockSpecies(); ++i) if (i % 100 == 0) std::this_thread::yield(); }
		// Снимает блокировку.
		void unlockSpecies() { lockFlag.clear(std::memory_order_release); }
	private:
		// Для блокировки одновременного доступа из разных потоков (расчётного и интерфейсного).
		std::atomic_flag lockFlag;
	};
};