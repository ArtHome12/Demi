/* ===============================================================================
	������������� �������� ������ ����.
	���� � ���� ����������.
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
		Gene(const std::string& name, const std::vector<std::string> valuesVector);

		// ��� ������������� ������������ ����������� ����� ���� �������.
		Gene(const Gene& sourceGene);

		// ��������, ����� �������� ���� �����������.
		void operator=(const Gene& sourceGene) = delete;

		const std::string& getGeneName() const { return geneName; }

	private:
		// �������� ����.
		std::string geneName;

		// ������ ��������, ������� ����� ��������� ���. ���� ������ ����, �� �� 0 �� 65535. ���� �������� ���� �������� � ����.
		std::vector<std::string> geneValuesVector;
	};


	//
	// ����� ��� �������� �������� - ������������ �����, �������������� ������������.
	//
	class Genotype {
	public:
		Genotype(const std::shared_ptr<Genotype>& aGenotypeAncestor, const Gene& gene, const std::string& aGenotypeName, const std::string& aGenotypeAuthor);

		// ���������� ��� ���� ���������.
		std::string getGenotypeName();

		// ���������� ��������� ���.
		const Gene& getGeneByName(const std::string& name);

		// ���������� ���������� �����.
		size_t getGenotypeLength() const { return cachedGenotypeLen; }

		const std::shared_ptr<Genotype>& getAncestor() const { return genotypeAncestor; }

	private:
		// ������������ �������.
		const std::shared_ptr<Genotype> genotypeAncestor;

		// ���, ������� ��������� ������� ���������� �� �������������.
		Gene ownGene;

		// �������� ������ �������� (���������).
		std::string genotypeName;

		// �����
		std::string genotypeAuthor;

		// ��� ���������.
		size_t cachedGenotypeLen = 0;
	};



	//
	// ��� ���������, �� ���� ������� � ����������� ���������� �����. 
	//
	class Species
	{
	public:
		// ����������� ��� ��������������
		Species(const std::shared_ptr<Genotype>& genotype,
			geneValues_t geneValue,
			bool Avisible,
			const clan::Color& AaliveColor,
			const clan::Color& AdeadColor
			);

		// ����������� ��� ���������� �� �����.
		Species(const std::shared_ptr<Genotype>& genotype, clan::File& binFile);

		// ������ � �����.
		void setVisible(bool AVisible) { visible = AVisible; };
		bool getVisible() { return visible; }
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }
		const std::shared_ptr<Genotype>& getGenotype() const { return speciesGenotype; }

		// ���������� ������������� ������ ��� ��������� ������ ��� ����������� ��������������.
		std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

		// ���������� ������ �������� ���� � ������� �����/���\�����/���... �������� ����� ��� ���� ��� �� ����������.
		//std::string getAuthorAndNamePair() { return author + "\\" + name + "/"; }
		//std::string getFullName();

		// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
		//std::shared_ptr<Species> getSpeciesByFullName(std::string fullName);

		// ���������� ��� �������� ���������.
		std::string getGenotypeName() { return speciesGenotype->getGenotypeName(); }

		// ��� ������� ���������� ���������� �� ���������� �����.
		void incAliveCount() { ++aliveCount; }
		void decAliveCount() { --aliveCount; }
		size_t getAliveCount() const { return aliveCount; }

		// ���������� �������� ���������� ����.
		geneValues_t getGeneValueByName(const std::string& name);

		// ��������� ���� � ����.
		void saveToFile(clan::File& binFile);

	private:
		// �������.
		const std::shared_ptr<Genotype> speciesGenotype;

		// �������� �����.
		std::vector<geneValues_t> geneValues;

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