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
		Gene(const std::string& name, const std::vector<std::string> valuesVector);

		// При использовании конструктора копирования могут быть мутации.
		Gene(const Gene& sourceGene);

		// Временно, потом возможно надо реализовать.
		void operator=(const Gene& sourceGene) = delete;

		const std::string& getGeneName() const { return geneName; }

	private:
		// Название гена.
		std::string geneName;

		// Список значений, которые может принимать ген. Если список пуст, то от 0 до 65535. Само значение гена хранится в виде.
		std::vector<std::string> geneValuesVector;
	};


	//
	// Класс для описания генотипа - совокупности генов, организованных иерархически.
	//
	class Genotype {
	public:
		Genotype(const std::shared_ptr<Genotype>& aGenotypeAncestor, const Gene& gene, const std::string& aGenotypeName, const std::string& aGenotypeAuthor);

		// Возвращает имя вида организма.
		std::string getGenotypeName();

		// Возвращает требуемый ген.
		const Gene& getGeneByName(const std::string& name);

		// Возвращает количество генов.
		size_t getGenotypeLength() const { return cachedGenotypeLen; }

		const std::shared_ptr<Genotype>& getAncestor() const { return genotypeAncestor; }

	private:
		// Родительский генотип.
		const std::shared_ptr<Genotype> genotypeAncestor;

		// Ген, которым настоящий генотип отличается от родительского.
		Gene ownGene;

		// Название нового генотипа (организма).
		std::string genotypeName;

		// Автор
		std::string genotypeAuthor;

		// Для ускорения.
		size_t cachedGenotypeLen = 0;
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
		Species(const std::shared_ptr<Genotype>& genotype, clan::File& binFile);

		// Доступ к полям.
		void setVisible(bool AVisible) { visible = AVisible; };
		bool getVisible() { return visible; }
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }
		const std::shared_ptr<Genotype>& getGenotype() const { return speciesGenotype; }

		// Возвращаем неконстантные ссылки для возможной правки для оптимизации быстродействия.
		std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

		// Возвращает полное название вида в формате автор/вид\автор/вид... Корневой общий для всех вид не включается.
		//std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
		//std::string getFullName();

		// Возвращает вид по указанному полному названию. Должна вызываться для корневого вида.
		//std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);

		// Возвращает имя генотипа организма.
		std::string getGenotypeName() { return speciesGenotype->getGenotypeName(); }

		// Для ведения простейшей статистики по количеству живых.
		void incAliveCount() { ++aliveCount; }
		void decAliveCount() { --aliveCount; }
		size_t getAliveCount() const { return aliveCount; }

		// Возвращает значение требуемого гена.
		geneValues_t getGeneValueByName(const std::string& name);

		// Сохраняет себя в файл.
		void saveToFile(clan::File& binFile);

	private:
		// Генотип.
		const std::shared_ptr<Genotype> speciesGenotype;

		// Значения генов.
		std::vector<geneValues_t> geneValues;

		// Клетки организма.
		std::vector<std::shared_ptr<GenericCell>> cells;

		// Видимость вида.
		bool visible;

		// Цвет для живого и мёртвого организмов.
		clan::Color aliveColor, deadColor;

		// Количество живых организмов данного вида (для статистики).
		size_t aliveCount = 0;
	};
};