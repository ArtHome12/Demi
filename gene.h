/* ===============================================================================
	������������� �������� ������ ����.
	���� � ���� ����������.
	05 october 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once


namespace demi {

	// ����������, ���� ��� �� ������.
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
	// ����� ��� �������� ������ ����.
	//
	class Gene {
	public:
		// �������� ������ ����.
		Gene(const std::string& name, const std::vector<std::string> valuesVector, uint16_t valueIndex);

		// ��� ������������� ������������ ����������� ����� ���� �������.
		Gene(const Gene& sourceGene);

		// ��������, ����� �������� ���� �����������.
		void operator=(const Gene& sourceGene) = delete;

		const std::string& getGeneName() const { return geneName; }
		uint16_t getGeneValueIndex() const { return geneValueIndex; }

	private:
		// �������� ����.
		std::string geneName;

		// ������ ��������, ������� ����� ��������� ���. ���� ������ ����, �� �� 0 �� 65535.
		std::vector<std::string> geneValuesVector;

		// ������ �������� �������� �� ������.
		uint16_t geneValueIndex = 0;
	};


	//
	// ����� ��� �������� �������� - ������������ �����, �������������� ������������.
	//
	class Genotype {
	public:
		Genotype(const std::shared_ptr<Genotype>& aGenotypeAncestor, const Gene& gene, const std::string& aGenotypeName, const std::string& aGenotypeAuthor);

		// ���������� ��� ���� ���������.
		std::string getSpeciesName();

		// ���������� ��������� ���.
		const Gene& getGeneByName(const std::string& name);
	private:
		// ������������ �������.
		const std::shared_ptr<Genotype> genotypeAncestor;

		// ���, ������� ��������� ������� ���������� �� �������������.
		Gene ownGene;

		// �������� ������ �������� (���������).
		std::string genotypeName;

		// �����
		std::string genotypeAuthor;
	};



	//
	// ��� ���������. ������ ��� �������� ������ ���� ����� ���. ����� ������ ��� �������� ������ �������.
	//
	class Species
	{
	public:
		Species(const std::shared_ptr<Genotype>& genotype,
			bool Avisible,
			const clan::Color& AaliveColor,
			const clan::Color& AdeadColor
			);

		// ������ � �����.
		void setVisible(bool AVisible) { visible = AVisible; };
		bool getVisible() { return visible; }
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }

		// ���������� ������������� ������ ��� ��������� ������ ��� ����������� ��������������.
		std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

		// ���������� ������ �������� ���� � ������� �����/���\�����/���... �������� ����� ��� ���� ��� �� ����������.
		//std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
		//std::string getFullName();

		// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
		std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);

		// ���������� ��� ���� ���������.
		std::string getSpeciesName() { return speciesGenotype->getSpeciesName(); }

		// ��� ������� ���������� ���������� �� ���������� �����.
		void incAliveCount() { ++aliveCount; }
		void decAliveCount() { --aliveCount; }
		size_t getAliveCount() const { return aliveCount; }

		// ���������� ��������� ���.
		const Gene& getGeneByName(const std::string& name) { return speciesGenotype->getGeneByName(name); }

	private:
		// �������.
		const std::shared_ptr<Genotype> speciesGenotype;

		// �������� ���������. ������ ������ ��������� ��� ����� �� ��� �����������, �� ���������� ����������� ���������
		// ���������� ��� ������ ������ �� ����, ������� ���������� � �����, ��-��������� ������������, �������� ����� ���.
		// ���� �������� ����� ������� ������������ ��������� ��� �������� ���������� � ������ - ������� Species, ��� ����,
		// � ������ ����� ������.
		//std::vector<std::shared_ptr<Species>> descendants;

		// ������ ���������.
		std::vector<std::shared_ptr<GenericCell>> cells;

		// ��������� ����.
		bool visible;

		// ���� ��� ������ � ������� ����������.
		clan::Color aliveColor, deadColor;

		// ���������� ����� ���������� ������� ���� (��� ����������).
		size_t aliveCount = 0;
	};
};