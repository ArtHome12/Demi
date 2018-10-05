/* ===============================================================================
	Моделирование эволюции живого мира.
	Гены и виды организмов.
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
// Класс для описания одного гена.
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
	// Название и список значений копируем без изменений, желательно с минимальными накладными расходами.
	// При копировании значения гена включаем механизм мутаций.
	geneValueIndex = sourceGene.geneValueIndex;	// не реализовано ещё.
}


/////////////////////////////////////////////////////////////////////////////////
// Класс для описания генотипа - совокупности генов, организованных иерархически.
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
	// Возвращает имя вида организма.
	return genotypeName + "(" + genotypeAuthor + ")";
}


/////////////////////////////////////////////////////////////////////////////////
// Вид организма
/////////////////////////////////////////////////////////////////////////////////
Species::Species(const std::shared_ptr<Genotype>& genotype,
	bool Avisible,
	const clan::Color& AaliveColor,
	const clan::Color& AdeadColor
) : speciesGenotype(genotype), visible(Avisible), aliveColor(AaliveColor), deadColor(AdeadColor)
{
	// При использовании конструктора инициализации по ссылке будет работать механизм мутаций.
}


// Возвращает полное название вида в формате автор/вид\автор/вид.../ Корневой общий для всех вид не включается.
//std::string Species::getFullName()
//{
//	// Если мы в корневом виде, то выходим.
//	auto spAncestor = ancestor.lock();
//	if (!spAncestor) return "";
//
//	// Если предок есть, то вернём предка и свои данные. 
//	return spAncestor->getFullName() + getAuthorAndNamePair();
//}


// Возвращает вид по указанному полному названию. Должна вызываться для корневого вида.
//std::shared_ptr<Species> Species::getSpeciesByFullName(std::string fullName)
//{
//	return nullptr;
//}



