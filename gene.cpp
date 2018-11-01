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
#include "gene.h"
#include "genotypes_tree.h"
#include "organism.h"
#include "world.h"

using namespace demi;

/////////////////////////////////////////////////////////////////////////////////
// Класс для описания одного гена.
/////////////////////////////////////////////////////////////////////////////////
Gene::Gene(const std::string& name, const std::vector<std::string>& valuesVector) :
	geneName(name),
	geneValuesVector(valuesVector)
{
}

// Возвращает значение гена в текстовом виде.
const std::string Gene::getGeneTextValue(geneValues_t numValue) const
{ 
	// Если список пуст, значит текстовое представление совпадает с индексом, то есть числовое значение.
	return geneValuesVector.empty() ? IntToStrWithDigitPlaces<geneValues_t>(numValue) : geneValuesVector.at(numValue);
}

// Возвращает максимальное значение гена.
geneValues_t Gene::getGeneMaxValue() const 
{ 
	// Максимальное значение гена определяется количеством возможных вариантов.
	size_t size = geneValuesVector.size();

	return geneValues_t(size ? size - 1 : 1);// geneValues_t_MAX);
}


/////////////////////////////////////////////////////////////////////////////////
// Класс для описания генотипа - совокупности генов, организованных иерархически.
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
	// Для ускорения рассчитаем часто используемый параметр.
	std::shared_ptr<Genotype>& anc = getAncestor();
	cachedGenotypeLen = (anc ? anc->cachedGenotypeLen : 0) + 1;
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
	if (treeNode->ancestor)
		treeNode->ancestor->genotype->getGeneByName(name);

	// Если более общего вида нет, сообщаем об ошибке.
	throw EGeneNotFound(name);
}


// Для ведения простейшей статистики по количеству живых.
void Genotype::incAliveCount()
{ 
	// Изменяем собственный счётчик.
	++aliveCount; 

	// Если есть более общий вид, уведомляем его.
	if (treeNode->ancestor)
		treeNode->ancestor->genotype->incAliveCount();
}

void Genotype::decAliveCount() 
{ 
	--aliveCount; 
	if (treeNode->ancestor)
		treeNode->ancestor->genotype->decAliveCount();
}

// Возвращает предшественника или nullptr.
std::shared_ptr<Genotype> Genotype::getAncestor() const
{ 
	// Указатель на вышустоящий узел дерева видов.
	auto& anc = treeNode->ancestor;

	// Если не пуст, вернём генотип узла-предка.
	return anc ? anc->genotype : nullptr;
}


/////////////////////////////////////////////////////////////////////////////////
// Вид организма
/////////////////////////////////////////////////////////////////////////////////
Species::Species(const std::shared_ptr<Genotype>& genotype,
	geneValues_t geneValue,
	bool Avisible
) : speciesGenotype(genotype), visible(Avisible), aliveColor(genotype->getAliveColor()), deadColor(genotype->getDeadColor())
{
	// Клетка вида. Требует переработки в будущем после появления клеток разных видов.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

	// Сохраняем значение гена.
	geneValues.push_back(geneValue);

	// Для оптимизации.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	initGeneValuesMax();
}


// Конструктор для считывания из файла.
Species::Species(const std::shared_ptr<Genotype>& genotype,	clan::IODevice& binFile) : speciesGenotype(genotype)
{
	// Клетка вида. Требует переработки в будущем после появления клеток разных видов.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

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

	// Для оптимизации.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	initGeneValuesMax();
}

// Конструктор для использования при мутации для создания производного вида.
Species::Species(const Species& ancestor, const std::shared_ptr<std::vector<geneValues_t>>& newGeneValues)
	: speciesGenotype(ancestor.speciesGenotype), visible(ancestor.visible), aliveColor(ancestor.aliveColor), deadColor(ancestor.deadColor), geneValues(*newGeneValues.get())
{
	// Клетка вида. Требует переработки в будущем после появления клеток разных видов.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

	// Для оптимизации.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	initGeneValuesMax();
}


// Конструктор для использования при мутации для создания первого вида для генотипа.
Species::Species(const std::shared_ptr<Genotype>& genotype, const Species& ancestor)
	: speciesGenotype(genotype), visible(ancestor.visible), aliveColor(genotype->getAliveColor()), deadColor(genotype->getDeadColor()), geneValues(ancestor.geneValues), geneValuesMax(ancestor.geneValuesMax)
{
	// Клетка вида. Требует переработки в будущем после появления клеток разных видов.
	cells.push_back(std::make_shared<demi::CellAbdomen>());

	// Добавляем значение для нового гена в интервале от нуля до максимального.
	std::uniform_int_distribution<> rnd_Elem(0, genotype->getOwnGeneMaxValue());
	geneValues.push_back(geneValues_t(rnd_Elem(globalWorld.getRandomGenerator())));

	// Для оптимизации.
	geneValuesCopy = std::make_shared<std::vector<geneValues_t>>(geneValues);
	geneValuesMax.push_back(genotype->getOwnGeneMaxValue());
}


// Возвращает значение требуемого гена.
geneValues_t Species::getGeneValueByName(const std::string& name) const
{
	// Имена генов у нас в иерархической цепочке в генотипе, а значения в векторе. Собираем всё вместе, 
	// синхронно разнонаправленно двигаясь по вектору и по дереву (имена генов от последнего к протоорганизму, значения наоборот).
	std::shared_ptr<Genotype> curGenotype = speciesGenotype;

	for (auto value = geneValues.rbegin(); value != geneValues.rend(); ++value) {
	
		// Если совпадает имя гена, возвращаем значение.
		if (curGenotype->getOwnGeneName() == name)
			return *value;

		// Переходим к новому значению автоматически в цикле и вручную к новому гену.
		curGenotype = curGenotype->getAncestor();
	}

	// Если не нашли названия, выбрасываем исключение.
	throw EGeneNotFound(name);
}

// Выводит имена генов и их значения, что и определяет имя вида.
std::string Species::getSpeciesName() const
{
	// Имена генов у нас в иерархической цепочке в генотипе, а значения в векторе. Собираем всё вместе, 
	// синхронно разнонаправленно двигаясь по вектору и по дереву (имена генов от последнего к протоорганизму, значения наоборот).
	std::string retVal;
	std::shared_ptr<Genotype> curGenotype = speciesGenotype;

	for (auto value = geneValues.rbegin(); value != geneValues.rend(); ++value) {

		// Вставляем в начало строки пару имя+текстовое значение.
		retVal = " [" + curGenotype->getOwnGeneName() + ": " + curGenotype->getOwnGeneTextValue(*value) + "]" + retVal;

		// Переходим к новому значению автоматически в цикле и вручную к новому гену.
		curGenotype = curGenotype->getAncestor();
	}

	return retVal;
}

// Сохраняет себя в файл.
void Species::saveToFile(clan::IODevice& binFile)
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

// Запускает механизм мутации и если она случилась, возвращает новые значения генов, а если нет, то nullptr.
std::shared_ptr<std::vector<geneValues_t>> Species::breeding() 
{
	// Флаг наличия изменений.
	bool changed = false;

	// Перебираем все значения генов.
	const size_t len = speciesGenotype->getGenotypeLength();	// Более быстрый способ, чем спрашивать сам вектор о размере.
	for (size_t i = 0; i != len; ++i) {

		// Определяем вероятность мутации.
		if (globalWorld.activateMutation()) {

			// Надо определить новое значение в интервале от нуля до максимального.
			std::uniform_int_distribution<> rnd_Elem(0, geneValuesMax[i]);
			(*geneValuesCopy)[i] = geneValues_t(rnd_Elem(globalWorld.getRandomGenerator()));

			// Если старое и новое значение не равны, поднимем флаг.
			if ((*geneValuesCopy)[i] != geneValues[i])
				changed = true;
		}
		else
			// Если мутации не будет, просто скопируем прежнее значение (пригодится при мутации в других генах).
			(*geneValuesCopy)[i] = geneValues[i];
	}

	// Если мутации были, возвращаем изменённый вектор значений.
	return changed ? geneValuesCopy : nullptr;
}


// Инициализирует geneValuesMax.
void Species::initGeneValuesMax()
{
	// Гены у нас в иерархической цепочке. Соберём их значения.
	std::shared_ptr<demi::GenotypesTree> curNode = speciesGenotype->getTreeNode();
	while (curNode) {
		// Ген текущего узла.
		geneValuesMax.push_back(curNode->genotype->getOwnGeneMaxValue());
		curNode = curNode->ancestor;
	}
}

bool Species::isTheSameGeneValues(const std::vector<geneValues_t>& otherValues) const
{
	// Сравним два вектора. Для стандатного оператора сравнения выполняется множество действий для поддержки собственных операторов сравнения у типов, поэтому упростим.
	//return geneValues-> == otherValues;
	return memcmp(geneValues.data(), otherValues.data(), speciesGenotype->getGenotypeLength() * sizeof(geneValues_t)) == 0;
}


