/* ===============================================================================
	Моделирование эволюции живого мира.
	Гены и виды организмов.
	05 october 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once


namespace demi {

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
		Gene(const std::string& name, const std::vector<std::string> valuesVector, uint16_t valueIndex);

		// При использовании конструктора копирования могут быть мутации.
		Gene(const Gene& sourceGene);

		// Временно, потом возможно надо реализовать.
		void operator=(const Gene& sourceGene) = delete;

		const std::string& getGeneName() const { return geneName; }
		uint16_t getGeneValueIndex() const { return geneValueIndex; }

	private:
		// Название гена.
		std::string geneName;

		// Список значений, которые может принимать ген. Если список пуст, то от 0 до 65535.
		std::vector<std::string> geneValuesVector;

		// Индекс текущего значения из списка.
		uint16_t geneValueIndex = 0;
	};


	//
	// Класс для описания генотипа - совокупности генов, организованных иерархически.
	//
	class Genotype {
	public:
		Genotype(const std::shared_ptr<Genotype>& aGenotypeAncestor, const Gene& gene, const std::string& aGenotypeName, const std::string& aGenotypeAuthor);

		// Возвращает имя вида организма.
		std::string getSpeciesName();

		// Возвращает требуемый ген.
		const Gene& getGeneByName(const std::string& name);
	private:
		// Родительский генотип.
		const std::shared_ptr<Genotype> genotypeAncestor;

		// Ген, которым настоящий генотип отличается от родительского.
		Gene ownGene;

		// Название нового генотипа (организма).
		std::string genotypeName;

		// Автор
		std::string genotypeAuthor;
	};



	//
	// Вид организма. Каждый вид содержит только один новый ген. Самый первый вид содержит только реакцию.
	//
	class Species
	{
	public:
		Species(const std::shared_ptr<Genotype>& genotype,
			bool Avisible,
			const clan::Color& AaliveColor,
			const clan::Color& AdeadColor
			);

		// Доступ к полям.
		void setVisible(bool AVisible) { visible = AVisible; };
		bool getVisible() { return visible; }
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }

		// Возвращаем неконстантные ссылки для возможной правки для оптимизации быстродействия.
		std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

		// Возвращает полное название вида в формате автор/вид\автор/вид... Корневой общий для всех вид не включается.
		//std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
		//std::string getFullName();

		// Возвращает вид по указанному полному названию. Должна вызываться для корневого вида.
		std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);

		// Возвращает имя вида организма.
		std::string getSpeciesName() { return speciesGenotype->getSpeciesName(); }

		// Для ведения простейшей статистики по количеству живых.
		void incAliveCount() { ++aliveCount; }
		void decAliveCount() { --aliveCount; }
		size_t getAliveCount() const { return aliveCount; }

		// Возвращает требуемый ген.
		const Gene& getGeneByName(const std::string& name) { return speciesGenotype->getGeneByName(name); }

	private:
		// Генотип.
		const std::shared_ptr<Genotype> speciesGenotype;

		// Дочерние организмы. Вообще самому организму нет нужды до его производных, но сохранение древовидной структуры
		// необходимо при записи данных на диск, поэтому информация о детях, во-избежание громоздкости, хранится прямо тут.
		// Хотя возможно стоит создать параллельную структуру для хранения информации в дереве - текущий Species, его дети,
		// а отсюда детей убрать.
		//std::vector<std::shared_ptr<Species>> descendants;

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