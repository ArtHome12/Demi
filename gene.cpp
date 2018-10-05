/* ===============================================================================
	������������� �������� ������ ����.
	���� � ���� ����������.
	05 october 2014.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
	Copyright (c) 2013 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "reactions.h"
#include "organism.h"
#include "gene.h"

using namespace demi;

/////////////////////////////////////////////////////////////////////////////////
// ����� ��� �������� ������ ����.
/////////////////////////////////////////////////////////////////////////////////
Gene::Gene(const std::string& name, const std::vector<std::string> valuesVector, uint16_t valueIndex) :
	geneName(name),
	geneValuesVector(valuesVector),
	geneValueIndex(valueIndex)
{

}

Gene::Gene(const Gene& sourceGene) : 
	geneName(sourceGene.geneName),
	geneValuesVector(sourceGene.geneValuesVector)
{
	// �������� � ������ �������� �������� ��� ���������, ���������� � ������������ ���������� ���������.
	// ��� ����������� �������� ���� �������� �������� �������.
	geneValueIndex = sourceGene.geneValueIndex;	// �� ����������� ���.
}


/////////////////////////////////////////////////////////////////////////////////
// ����� ��� �������� �������� - ������������ �����, �������������� ������������.
/////////////////////////////////////////////////////////////////////////////////
Genotype::Genotype(const std::shared_ptr<Genotype>& aGenotypeAncestor, const Gene& gene, const std::string& aGenotypeName, const std::string& aGenotypeAuthor) :
	genotypeAncestor(aGenotypeAncestor),
	ownGene(gene),
	genotypeName(aGenotypeName),
	genotypeAuthor(aGenotypeAuthor)
{

}

std::string Genotype::getSpeciesName()
{
	// ���������� ��� ���� ���������.
	return genotypeName + "(" + genotypeAuthor + ")";
}


/////////////////////////////////////////////////////////////////////////////////
// ��� ���������
/////////////////////////////////////////////////////////////////////////////////
Species::Species(const std::shared_ptr<Genotype>& genotype,
	bool Avisible,
	const clan::Color& AaliveColor,
	const clan::Color& AdeadColor
) : speciesGenotype(genotype), visible(Avisible), aliveColor(AaliveColor), deadColor(AdeadColor)
{
	// ��� ������������� ������������ ������������� �� ������ ����� �������� �������� �������.
}


// ���������� ������ �������� ���� � ������� �����/���\�����/���.../ �������� ����� ��� ���� ��� �� ����������.
//std::string Species::getFullName()
//{
//	// ���� �� � �������� ����, �� �������.
//	auto spAncestor = ancestor.lock();
//	if (!spAncestor) return "";
//
//	// ���� ������ ����, �� ����� ������ � ���� ������. 
//	return spAncestor->getFullName() + getAuthorAndNamePair();
//}


// ���������� ��� �� ���������� ������� ��������. ������ ���������� ��� ��������� ����.
//std::shared_ptr<Species> Species::getSpeciesByFullName(std::string fullName)
//{
//	return nullptr;
//}



