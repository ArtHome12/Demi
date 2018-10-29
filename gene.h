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
#define geneValues_t_MAX UINT16_MAX


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
		Gene(const std::string& name, const std::vector<std::string>& valuesVector);

		// ���������� ��� ����.
		const std::string& getGeneName() const { return geneName; }

		// ���������� �������� ���� � ��������� ���� (�� �� ������, �����-�� ��������).
		const std::string getGeneTextValue(geneValues_t numValue) const;

		// ���������� ������������ �������� ����.
		geneValues_t getGeneMaxValue() const;

	private:
		// �������� ����.
		std::string geneName;

		// ������ ��������, ������� ����� ��������� ���. ���� ������ ����, �� �� 0 �� 65535. ���� �������� ���� �������� � ����.
		std::vector<std::string> geneValuesVector;
	};


	//
	// ����� ��� �������� �������� - ������������ �����, �������������� ������������.
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

		// ���������� ��� ���� ��������� � ������� ���(�����).
		std::string getGenotypeName();

		// ���������� ��������� ���.
		const Gene& getGeneByName(const std::string& name);

		// ���������� ���������� �����.
		size_t getGenotypeLength() const { return cachedGenotypeLen; }

		// ���������� ��� ������������ ����.
		const std::string& getOwnGeneName() const { return ownGene.getGeneName(); }

		// ���������� ��������� �������� ������������ ����.
		const std::string getOwnGeneTextValue(geneValues_t numValue) const { return ownGene.getGeneTextValue(numValue); }

		// ���������� ������������ �������� ������������ ����.
		geneValues_t getOwnGeneMaxValue() const { return ownGene.getGeneMaxValue(); }

		// ������ �� ������.
		GenotypesTree& getTreeNode() const { return treeNode; }

		// ���������� ��������������� ��� nullptr.
		std::shared_ptr<Genotype> getAncestor() const;

		// ��� ������� ���������� ���������� �� ���������� �����.
		void incAliveCount();
		void decAliveCount();
		size_t getAliveCount() const { return aliveCount; }

		// ����� ��������.
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }

	private:
		// ���� ������ ���������, ����������� � ������� ��������.
		GenotypesTree& treeNode;

		// ���, ������� ��������� ������� ���������� �� �������������.
		Gene ownGene;

		// �������� ������ �������� (���������).
		std::string genotypeName;

		// �����
		std::string genotypeAuthor;

		// ��� ���������.
		size_t cachedGenotypeLen = 0;

		// ���������� ����� ���������� ������� ���� (��� ����������).
		size_t aliveCount = 0;

		// ���� ��� ������ � ������� ����������.
		clan::Color aliveColor, deadColor;
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
		Species(const std::shared_ptr<Genotype>& genotype, clan::IODevice& binFile);

		// ����������� ��� ������������� ��� ������� ��� �������� ������������ ����.
		Species(const Species& ancestor, const std::shared_ptr<std::vector<geneValues_t>>& newGeneValues);

		// ������ � �����.
		void setVisible(bool AVisible) { visible = AVisible; };
		bool getVisible() { return visible; }
		const clan::Color& getAliveColor() { return aliveColor; }
		const clan::Color& getDeadColor() { return deadColor; }
		const std::shared_ptr<Genotype>& getGenotype() const { return speciesGenotype; }

		// ���������� ������������� ������ ��� ��������� ������ ��� ����������� ��������������.
		std::vector<std::shared_ptr<GenericCell>>& getCellsRef() { return cells; }

		// ���������� ��� �������� ���������.
		std::string getGenotypeName() { return speciesGenotype->getGenotypeName(); }

		// ������� ����� ����� � �� ��������, ��� � ���������� ��� ����.
		std::string getSpeciesName() const;

		// ��� ������� ���������� ���������� �� ���������� �����.
		void incAliveCount() { ++aliveCount; speciesGenotype->incAliveCount(); }
		void decAliveCount() { --aliveCount; speciesGenotype->decAliveCount(); }
		size_t getAliveCount() const { return aliveCount; }

		// ���������� �������� ���������� ����.
		geneValues_t getGeneValueByName(const std::string& name) const;

		// ��������� ���� � ����.
		void saveToFile(clan::IODevice& binFile);

		// ��������� �������� ������� � ���� ��� ���������, ���������� ����� �������� �����, � ���� ���, �� nullptr.
		std::shared_ptr<std::vector<geneValues_t>> breeding();

		// ��������� ������, ���� �������� ����� ���������.
		bool isTheSameGeneValues(const std::shared_ptr<std::vector<geneValues_t>>& otherValues) const { return geneValues == *otherValues.get(); }

	private:
		// �������.
		const std::shared_ptr<Genotype> speciesGenotype;

		// �������� �����. ������ ��� ������� ���, ��������� ��� ��������������.
		std::vector<geneValues_t> geneValues;

		// ����� �������� ����� � �� ������������ �������� ��� ����������� ��� ������ breeding().
		std::shared_ptr<std::vector<geneValues_t>> geneValuesCopy;
		std::vector<geneValues_t> geneValuesMax;

		// ������ ���������.
		std::vector<std::shared_ptr<GenericCell>> cells;

		// ��������� ����.
		bool visible;

		// ���� ��� ������ � ������� ����������.
		clan::Color aliveColor, deadColor;

		// ���������� ����� ���������� ������� ���� (��� ����������).
		size_t aliveCount = 0;

		// �������������� geneValuesMax.
		void initGeneValuesMax();
	};
};