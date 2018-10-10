/* ===============================================================================
	������������� �������� ������ ����.
	���� � ���� ����������.
	05 october 2018.
	----------------------------------------------------------------------------
   Licensed under the terms of the GPL version 3.
   http://www.gnu.org/licenses/gpl-3.0.html
   Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "reactions.h"
#include "organism.h"
#include "gene.h"

using namespace demi;

/////////////////////////////////////////////////////////////////////////////////
// ����� ��� �������� ������ ����.
/////////////////////////////////////////////////////////////////////////////////
Gene::Gene(const std::string& name, const std::vector<std::string> valuesVector) :
	geneName(name),
	geneValuesVector(valuesVector)
{

}

Gene::Gene(const Gene& sourceGene) : 
	geneName(sourceGene.geneName),
	geneValuesVector(sourceGene.geneValuesVector)
{
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
	// ��� ��������� ���������� ����� ������������ ��������.
	cachedGenotypeLen = genotypeAncestor.get() != nullptr ? genotypeAncestor->getGenotypeLength() : 1;
}

std::string Genotype::getGenotypeName()
{
	// ���������� ��� ���� ���������.
	return genotypeName + "(" + genotypeAuthor + ")";
}

// ���������� ��������� ���.
const Gene& Genotype::getGeneByName(const std::string& name)
{
	// �������� ����������� ���, ���� ��, �� �������.
	if (ownGene.getGeneName() == name)
		return ownGene;

	// ���������� � ��������.
	if (genotypeAncestor != nullptr)
		return genotypeAncestor->getGeneByName(name);

	throw EGeneNotFound(name);
}


/////////////////////////////////////////////////////////////////////////////////
// ��� ���������
/////////////////////////////////////////////////////////////////////////////////
Species::Species(const std::shared_ptr<Genotype>& genotype,
	geneValues_t geneValue,
	bool Avisible,
	const clan::Color& AaliveColor,
	const clan::Color& AdeadColor
) : speciesGenotype(genotype), visible(Avisible), aliveColor(AaliveColor), deadColor(AdeadColor)
{
	// �������� � ������ �������� �������� ��� ���������, ���������� � ������������ ���������� ���������.
	// ��� ����������� �������� ���� �������� �������� �������.
	//geneValueIndex = sourceGene.geneValueIndex;	// �� ����������� ���.
	// ��� ������������� ������������ ������������� �� ������ ����� �������� �������� �������.
	geneValues.push_back(geneValue);
}


// ����������� ��� ���������� �� �����.
Species::Species(const std::shared_ptr<Genotype>& genotype,	clan::File& binFile) : speciesGenotype(genotype)
{
	// ����� ���.
	size_t len = genotype->getGenotypeLength();

	// ������� � �������� �������� �����.
	for (size_t i = 0; i != len; ++i)
		geneValues.push_back(binFile.read_uint16());

	// ���������.
	visible = binFile.read_int8() != 0;

	// �����.
	unsigned char r = binFile.read_uint8(), g = binFile.read_uint8(), b = binFile.read_uint8();
	aliveColor = clan::Color(r, g, b);
	r = binFile.read_uint8(); g = binFile.read_uint8(); b = binFile.read_uint8();
	deadColor = clan::Color(r, g, b);
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


// ���������� �������� ���������� ����.
geneValues_t Species::getGeneValueByName(const std::string& name) 
{ 
	return geneValues.back();
}

// ��������� ���� � ����.
void Species::saveToFile(clan::File& binFile)
{
	// �������� �����.
	for (geneValues_t geneValue : geneValues)
		binFile.write_uint16(geneValue);

	// ���������.
	binFile.write_int8(visible);

	// �����.
	binFile.write_uint8(aliveColor.get_red());
	binFile.write_uint8(aliveColor.get_green());
	binFile.write_uint8(aliveColor.get_blue());
	binFile.write_uint8(deadColor.get_red());
	binFile.write_uint8(deadColor.get_green());
	binFile.write_uint8(deadColor.get_blue());
}
