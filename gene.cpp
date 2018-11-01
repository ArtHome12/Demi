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
#include "gene.h"
#include "genotypes_tree.h"
#include "organism.h"
#include "world.h"

using namespace demi;

/////////////////////////////////////////////////////////////////////////////////
// ����� ��� �������� ������ ����.
/////////////////////////////////////////////////////////////////////////////////
Gene::Gene(const std::string& name, const std::vector<std::string>& valuesVector) :
	geneName(name),
	geneValuesVector(valuesVector)
{
}

// ���������� �������� ���� � ��������� ����.
const std::string Gene::getGeneTextValue(geneValues_t numValue) const
{ 
	// ���� ������ ����, ������ ��������� ������������� ��������� � ��������, �� ���� �������� ��������.
	return geneValuesVector.empty() ? IntToStrWithDigitPlaces<geneValues_t>(numValue) : geneValuesVector.at(numValue);
}

// ���������� ������������ �������� ����.
geneValues_t Gene::getGeneMaxValue() const 
{ 
	// ������������ �������� ���� ������������ ����������� ��������� ���������.
	size_t size = geneValuesVector.size();

	return geneValues_t(size ? size - 1 : 1);// geneValues_t_MAX);
}


/////////////////////////////////////////////////////////////////////////////////
// ����� ��� �������� �������� - ������������ �����, �������������� ������������.
/////////////////////////////////////////////////////////////////////////////////
Genotype::Genotype(std::shared_ptr<demi::GenotypesTree> aTreeNode,
	const Gene& gene,
	const std::string& aGenotypeName,
	const std::string& aGenotypeAuthor,
	const clan::Color& AaliveColor,
	const clan::Color& AdeadColor
) :
	treeNode(aTreeNode),
	ownGene(gene),
	genotypeName(aGenotypeName),
	genotypeAuthor(aGenotypeAuthor),
	aliveColor(AaliveColor), 
	deadColor(AdeadColor)
{
	// ��� ��������� ���������� ����� ������������ ��������.
	std::shared_ptr<Genotype>& anc = getAncestor();
	cachedGenotypeLen = (anc ? anc->cachedGenotypeLen : 0) + 1;
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
	if (treeNode->ancestor)
		treeNode->ancestor->genotype->getGeneByName(name);

	// ���� ����� ������ ���� ���, �������� �� ������.
	throw EGeneNotFound(name);
}


// ��� ������� ���������� ���������� �� ���������� �����.
void Genotype::incAliveCount()
{ 
	// �������� ����������� �������.
	++aliveCount; 

	// ���� ���� ����� ����� ���, ���������� ���.
	if (treeNode->ancestor)
		treeNode->ancestor->genotype->incAliveCount();
}

void Genotype::decAliveCount() 
{ 
	--aliveCount; 
	if (treeNode->ancestor)
		treeNode->ancestor->genotype->decAliveCount();
}

// ���������� ��������������� ��� nullptr.
std::shared_ptr<Genotype> Genotype::getAncestor() const
{ 
	// ��������� �� ����������� ���� ������ �����.
	auto& anc = treeNode->ancestor;

	// ���� �� ����, ����� ������� ����-������.
	return anc ? anc->genotype : nullptr;
}


/////////////////////////////////////////////////////////////////////////////////
// ��� ���������
/////////////////////////////////////////////////////////////////////////////////
Species::Species(const std::shared_ptr<Genotype>& genotype,
	geneValues_t geneValue,
	bool Avisible
) : speciesGenotype(genotype), visible(Avisible), aliveColor(genotype->getAliveColor()), deadColor(genotype->getDeadColor())
{
	// ������ ����. ������� ����������� � ������� ����� ��������� ������ ������ �����.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

	// ��������� �������� ����.
	geneValues.push_back(geneValue);

	// ��� �����������.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	initGeneValuesMax();
}


// ����������� ��� ���������� �� �����.
Species::Species(const std::shared_ptr<Genotype>& genotype,	clan::IODevice& binFile) : speciesGenotype(genotype)
{
	// ������ ����. ������� ����������� � ������� ����� ��������� ������ ������ �����.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

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

	// ��� �����������.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	initGeneValuesMax();
}

// ����������� ��� ������������� ��� ������� ��� �������� ������������ ����.
Species::Species(const Species& ancestor, const std::shared_ptr<std::vector<geneValues_t>>& newGeneValues)
	: speciesGenotype(ancestor.speciesGenotype), visible(ancestor.visible), aliveColor(ancestor.aliveColor), deadColor(ancestor.deadColor), geneValues(*newGeneValues.get())
{
	// ������ ����. ������� ����������� � ������� ����� ��������� ������ ������ �����.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

	// ��� �����������.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	initGeneValuesMax();
}


// ����������� ��� ������������� ��� ������� ��� �������� ������� ���� ��� ��������.
Species::Species(const std::shared_ptr<Genotype>& genotype, const Species& ancestor)
	: speciesGenotype(genotype), visible(ancestor.visible), aliveColor(genotype->getAliveColor()), deadColor(genotype->getDeadColor()), geneValues(ancestor.geneValues), geneValuesMax(ancestor.geneValuesMax)
{
	// ������ ����. ������� ����������� � ������� ����� ��������� ������ ������ �����.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

	// ��������� �������� ��� ������ ���� � ��������� �� ���� �� �������������.
	std::uniform_int_distribution<> rnd_Elem(0, genotype->getOwnGeneMaxValue());
	geneValues.push_back(geneValues_t(rnd_Elem(globalWorld.getRandomGenerator())));

	// ��� �����������.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	geneValuesMax.push_back(genotype->getOwnGeneMaxValue());
}


// ���������� �������� ���������� ����.
geneValues_t Species::getGeneValueByName(const std::string& name) const
{
	// ����� ����� � ��� � ������������� ������� � ��������, � �������� � �������. �������� �� ������, 
	// ��������� ���������������� �������� �� ������� � �� ������ (����� ����� �� ���������� � ��������������, �������� ��������).
	std::shared_ptr<Genotype> curGenotype = speciesGenotype;

	for (auto value = geneValues.rbegin(); value != geneValues.rend(); ++value) {
	
		// ���� ��������� ��� ����, ���������� ��������.
		if (curGenotype->getOwnGeneName() == name)
			return *value;

		// ��������� � ������ �������� ������������� � ����� � ������� � ������ ����.
		curGenotype = curGenotype->getAncestor();
	}

	// ���� �� ����� ��������, ����������� ����������.
	throw EGeneNotFound(name);
}

// ������� ����� ����� � �� ��������, ��� � ���������� ��� ����.
std::string Species::getSpeciesName() const
{
	// ����� ����� � ��� � ������������� ������� � ��������, � �������� � �������. �������� �� ������, 
	// ��������� ���������������� �������� �� ������� � �� ������ (����� ����� �� ���������� � ��������������, �������� ��������).
	std::string retVal;
	std::shared_ptr<Genotype> curGenotype = speciesGenotype;

	for (auto value = geneValues.rbegin(); value != geneValues.rend(); ++value) {

		// ��������� � ������ ������ ���� ���+��������� ��������.
		retVal = " [" + curGenotype->getOwnGeneName() + ": " + curGenotype->getOwnGeneTextValue(*value) + "]" + retVal;

		// ��������� � ������ �������� ������������� � ����� � ������� � ������ ����.
		curGenotype = curGenotype->getAncestor();
	}

	return retVal;
}

// ��������� ���� � ����.
void Species::saveToFile(clan::IODevice& binFile)
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

// ��������� �������� ������� � ���� ��� ���������, ���������� ����� �������� �����, � ���� ���, �� nullptr.
std::shared_ptr<std::vector<geneValues_t>> Species::breeding() 
{
	// ���� ������� ���������.
	bool changed = false;

	// ���������� ��� �������� �����.
	const size_t len = speciesGenotype->getGenotypeLength();	// ����� ������� ������, ��� ���������� ��� ������ � �������.
	for (size_t i = 0; i != len; ++i) {

		// ���������� ����������� �������.
		if (globalWorld.activateMutation()) {

			// ���� ���������� ����� �������� � ��������� �� ���� �� �������������.
			std::uniform_int_distribution<> rnd_Elem(0, geneValuesMax[i]);
			(*geneValuesCopy)[i] = geneValues_t(rnd_Elem(globalWorld.getRandomGenerator()));

			// ���� ������ � ����� �������� �� �����, �������� ����.
			if ((*geneValuesCopy)[i] != geneValues[i])
				changed = true;
		}
		else
			// ���� ������� �� �����, ������ ��������� ������� �������� (���������� ��� ������� � ������ �����).
			(*geneValuesCopy)[i] = geneValues[i];
	}

	// ���� ������� ����, ���������� ��������� ������ ��������.
	return changed ? geneValuesCopy : nullptr;
}


// �������������� geneValuesMax.
void Species::initGeneValuesMax()
{
	// ���� � ��� � ������������� �������. ������ �� ��������.
	std::shared_ptr<demi::GenotypesTree> curNode = speciesGenotype->getTreeNode();
	while (curNode) {
		// ��� �������� ����.
		geneValuesMax.push_back(curNode->genotype->getOwnGeneMaxValue());
		curNode = curNode->ancestor;
	}
}

bool Species::isTheSameGeneValues(const std::vector<geneValues_t>& otherValues) const
{
	// ������� ��� �������. ��� ����������� ��������� ��������� ����������� ��������� �������� ��� ��������� ����������� ���������� ��������� � �����, ������� ��������.
	//return geneValues-> == otherValues;
	return memcmp(geneValues.data(), otherValues.data(), speciesGenotype->getGenotypeLength() * sizeof(geneValues_t)) == 0;
}


