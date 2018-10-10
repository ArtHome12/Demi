/* ===============================================================================
	Моделирование эволюции живого мира.
	Гены и виды организмов.
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
// Класс для описания одного гена.
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
// Класс для описания генотипа - совокупности генов, организованных иерархически.
/////////////////////////////////////////////////////////////////////////////////
Genotype::Genotype(const std::shared_ptr<Genotype>& aGenotypeAncestor, const Gene& gene, const std::string& aGenotypeName, const std::string& aGenotypeAuthor) :
	genotypeAncestor(aGenotypeAncestor),
	ownGene(gene),
	genotypeName(aGenotypeName),
	genotypeAuthor(aGenotypeAuthor)
{
	// Для ускорения рассчитаем часто используемый параметр.
	cachedGenotypeLen = genotypeAncestor.get() != nullptr ? genotypeAncestor->getGenotypeLength() : 1;
}

std::string Genotype::getGenotypeName()
{
	// Возвращает имя вида организма.
	return genotypeName + "(" + genotypeAuthor + ")";
}

// Возвращает требуемый ген.
const Gene& Genotype::getGeneByName(const std::string& name)
{
	// Проверим собственный ген, если он, то выходим.
	if (ownGene.getGeneName() == name)
		return ownGene;

	// Обращаемся к родителю.
	if (genotypeAncestor != nullptr)
		return genotypeAncestor->getGeneByName(name);

	throw EGeneNotFound(name);
}


/////////////////////////////////////////////////////////////////////////////////
// Вид организма
/////////////////////////////////////////////////////////////////////////////////
Species::Species(const std::shared_ptr<Genotype>& genotype,
	geneValues_t geneValue,
	bool Avisible,
	const clan::Color& AaliveColor,
	const clan::Color& AdeadColor
) : speciesGenotype(genotype), visible(Avisible), aliveColor(AaliveColor), deadColor(AdeadColor)
{
	// Название и список значений копируем без изменений, желательно с минимальными накладными расходами.
	// При копировании значения гена включаем механизм мутаций.
	//geneValueIndex = sourceGene.geneValueIndex;	// не реализовано ещё.
	// При использовании конструктора инициализации по ссылке будет работать механизм мутаций.
	geneValues.push_back(geneValue);
}


// Конструктор для считывания из файла.
Species::Species(const std::shared_ptr<Genotype>& genotype,	clan::File& binFile) : speciesGenotype(genotype)
{
	// Длина ДНК.
	size_t len = genotype->getGenotypeLength();

	// Считаем и сохраним значения генов.
	for (size_t i = 0; i != len; ++i)
		geneValues.push_back(binFile.read_uint16());

	// Видимость.
	visible = binFile.read_int8() != 0;

	// Цвета.
	unsigned char r = binFile.read_uint8(), g = binFile.read_uint8(), b = binFile.read_uint8();
	aliveColor = clan::Color(r, g, b);
	r = binFile.read_uint8(); g = binFile.read_uint8(); b = binFile.read_uint8();
	deadColor = clan::Color(r, g, b);
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


// Возвращает значение требуемого гена.
geneValues_t Species::getGeneValueByName(const std::string& name) 
{ 
	return geneValues.back();
}

// Сохраняет себя в файл.
void Species::saveToFile(clan::File& binFile)
{
	// Значения генов.
	for (geneValues_t geneValue : geneValues)
		binFile.write_uint16(geneValue);

	// Видимость.
	binFile.write_int8(visible);

	// Цвета.
	binFile.write_uint8(aliveColor.get_red());
	binFile.write_uint8(aliveColor.get_green());
	binFile.write_uint8(aliveColor.get_blue());
	binFile.write_uint8(deadColor.get_red());
	binFile.write_uint8(deadColor.get_green());
	binFile.write_uint8(deadColor.get_blue());
}
