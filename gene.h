/* ===============================================================================
	Моделирование эволюции живого мира.
	Гены и виды организмов.
	05 october 2018.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
   Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once

#include <vector>

namespace demi {

	typedef uint16_t geneValues_t;
#define geneValues_t_MAX UINT16_MAX


	// Исключение, если ген не найден.
	class EGeneNotFound : public std::exception
	{
	public:
		const std::string geneName;
		EGeneNotFound(const std::string& aGeneName) : geneName(aGeneName) {}
		virtual char const* what() const override
		{
			return std::string("Unknown gene: " + geneName).c_str();
		}
	};

	//
	// Класс для описания одного гена.
	//
	class Gene {
	public:
		// Создание нового гена.
		Gene(const std::string& name, const std::vector<std::string>& valuesVector);

		// Возвращает имя гена.
		const std::string& getGeneName() const { return geneName; }

		// Возвращает значение гена в текстовом виде (не по ссылке, какие-то проблемы).
		const std::string getGeneTextValue(geneValues_t numValue) const;

		// Возвращает максимальное значение гена.
		geneValues_t getGeneMaxValue() const;

	private:
		// Название гена.
		std::string geneName;

		// Список значений, которые может принимать ген. Если список пуст, то от 0 до 65535. Само значение гена хранится у вида.
		std::vector<std::string> geneValuesVector;
	};


	//
	// Класс для описания генотипа - совокупности генов, организованных иерархически.
	//
	class GenotypesTree;
	class Genotype {
	public:
		Genotype(GenotypesTree& aTreeNode, 
			const Gene& gene, 
			const std::string& aGenotypeName, 
			const std::string& aGenotypeAuthor, 
			const clan::Color& AaliveColor,
			const clan::Color& AdeadColor
		);

		Genotype(const Genotype& sourceGene) = delete;
		void operator=(const Genotype& sourceGene) = delete;

		// Возвращает имя вида организма в формате имя(автор).
		std::string getGenotypeName();

		// Возвращает требуемый ген.
		const Gene& getGeneByName(const std::string& name);

		// Возвращает количество генов.
		size_t getGenotypeLength() const { return cachedGenotypeLen; }

		// Возвращает имя собственного гена.
		const std::string& getOwnGeneName() const { return ownGene.getGeneName(); }

		// Возвращает текстовое значение собственного гена.
		const std::string getOwnGeneTextValue(geneValues_t numValue) const { return ownGene.getGeneTextValue(numValue); }

		// Возвращает максимальное значение собственного гена.
		geneValues_t getOwnGeneMaxValue() const { return ownGene.getGeneMaxValue(); }

		// Ссылка на дерево.
		GenotypesTree& getTreeNode() const { return treeNode; }

		// Возвращает предшественника или nullptr.
		std::shared_ptr<Genotype> getAncestor() const;

		// Для ведения простейшей статистики по количеству живых.
		void incAliveCount();
		void decAliveCount();
		size_t getAliveCount() const { return aliveCount; }

		// Цвета генотипа.
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }

	private:
		// Узел дерева генотипов, относящийся к данному генотипу.
		GenotypesTree& treeNode;

		// Ген, которым настоящий генотип отличается от родительского.
		Gene ownGene;

		// Название нового генотипа (организма).
		std::string genotypeName;

		// Автор
		std::string genotypeAuthor;

		// Для ускорения.
		size_t cachedGenotypeLen = 0;

		// Количество живых организмов данного вида (для статистики).
		size_t aliveCount = 0;

		// Цвет для живого и мёртвого организмов.
		clan::Color aliveColor, deadColor;
	};



	//
	// Вид организма, то есть генотип с конкретными значениями генов. 
	//
	class Species
	{
	public:
		// Конструктор для протоорганизма
		Species(const std::shared_ptr<Genotype>& genotype,
			geneValues_t geneValue,
			bool Avisible,
			const clan::Color& AaliveColor,
			const clan::Color& AdeadColor
			);

		// Конструктор для считывания из файла.
		Species(const std::shared_ptr<Genotype>& genotype, clan::IODevice& binFile);

		// Конструктор для использования при мутации для создания производного вида.
		Species(const Species& ancestor, const std::shared_ptr<std::vector<geneValues_t>>& newGeneValues);

		// Доступ к полям.
		void setVisible(bool AVisible) { visible = AVisible; };
		bool getVisible() { return visible; }
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }
		const std::shared_ptr<Genotype>& getGenotype() const { return speciesGenotype; }

		// Возвращаем неконстантные ссылки для возможной правки для оптимизации быстродействия.
		std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

		// Возвращает имя генотипа организма.
		std::string getGenotypeName() { return speciesGenotype->getGenotypeName(); }

		// Выводит имена генов и их значения, что и определяет имя вида.
		std::string getSpeciesName() const;

		// Для ведения простейшей статистики по количеству живых.
		void incAliveCount() { ++aliveCount; speciesGenotype->incAliveCount(); }
		void decAliveCount() { --aliveCount; speciesGenotype->decAliveCount(); }
		size_t getAliveCount() const { return aliveCount; }

		// Возвращает значение требуемого гена.
		geneValues_t getGeneValueByName(const std::string& name) const;

		// Сохраняет себя в файл.
		void saveToFile(clan::IODevice& binFile);

		// Запускает механизм мутации и если она случилась, возвращает новые значения генов, а если нет, то nullptr.
		std::shared_ptr<std::vector<geneValues_t>> breeding();

		// Возващает истину, если значения генов совпадают.
		bool isTheSameGeneValues(const std::shared_ptr<std::vector<geneValues_t>>& otherValues) const { return geneValues == *otherValues.get(); }

	private:
		// Генотип.
		const std::shared_ptr<Genotype> speciesGenotype;

		// Значения генов. Первым идёт текущий ген, последним ген протоорганизма.
		std::vector<geneValues_t> geneValues;

		// Копия значений генов и их максимальных значений для оптимизации при вызове breeding().
		std::shared_ptr<std::vector<geneValues_t>> geneValuesCopy;
		std::vector<geneValues_t> geneValuesMax;

		// Клетки организма.
		std::vector<std::shared_ptr<GenericCell>> cells;

		// Видимость вида.
		bool visible;

		// Цвет для живого и мёртвого организмов.
		clan::Color aliveColor, deadColor;

		// Количество живых организмов данного вида (для статистики).
		size_t aliveCount = 0;

		// Инициализирует geneValuesMax.
		void initGeneValuesMax();
	};
};