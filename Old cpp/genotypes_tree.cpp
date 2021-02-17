/* ===============================================================================
Моделирование эволюции живого мира.
Древовидная иерархия генотипов и видов для взаимодействия с пользователем.
10 october 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "genotypes_tree.h"
#include "gene.h"
#include "world.h"

using namespace demi;

bool GenotypesTree::flagSpaciesChanged = true;

// Создаёт словарь для сопоставления видов и индексов.
void GenotypesTree::generateDict(speciesDict_t& dict)
{
	// Помещаем собственные виды.
	for (const std::shared_ptr<Species> spec : species)
		dict.push_back(spec);

	// Вызываем рекурсивно для каждого производного узла.
	for (auto& derivative : derivatives)
		derivative->generateDict(dict);
}

// Записывает дерево на диск.
void GenotypesTree::saveToFile(clan::IODevice& binFile)
{
	// Количество собственных видов.
	uint32_t cnt = uint32_t(species.size());
	binFile.write_uint32(cnt);
	
	// Сами собственные виды.
	for (const std::shared_ptr<Species>& spec : species)
		spec->saveToFile(binFile);

	// Производные узлы (их количество предопределено из XML).
	for (auto& derivative : derivatives)
		derivative->saveToFile(binFile);
}

// Счтиывает себя с диска.
void GenotypesTree::loadFromFile(clan::IODevice& binFile)
{
	// Количество собственных видов.
	uint32_t cnt = binFile.read_uint32();

	// Сами собственные виды.
	for (uint32_t i = 0; i != cnt; ++i)
		species.push_back(std::make_shared<Species>(genotype, binFile));

	// Производные узлы (их количество предопределено из XML).
	for (auto& derivative : derivatives)
		derivative->loadFromFile(binFile);
}


// Возвращает указатель на вид в рамках текущего генотипа, возможно с мутацией.
const std::shared_ptr<Species> GenotypesTree::breeding(const std::shared_ptr<demi::Species>& oldSpec)
{
	// Получаем новые (мутировавшие) значения генов или nullptr, если мутаций не случилось.
	std::shared_ptr<std::vector<geneValues_t>> newGeneValues = oldSpec->breeding();

	// Если мутации в существующих генах не произошло, новые проверки.
	if (!newGeneValues) {

		// Необходимо вернуть вид с производным генотипом, если такие имеются.
		size_t num = derivatives.size();
		if (!num)
			// Возвращаем тот же самый вид, мутировать не в кого.
			return oldSpec;

		// Мутация с новым геном должна быть реже более заурядной, используем двойную вероятность.
		if (globalWorld.activateMutation() /*&& globalWorld.activateMutation()*/) {
			
			// Выберем из имеющихся производных генотипов.
			std::uniform_int_distribution<> rnd_Index(0, num - 1);
			size_t index = rnd_Index(globalWorld.getRandomGenerator());
			auto& newTree = derivatives[index];
			auto& derSpecs = newTree->species;

			// Для производного генотипа создаём новый вид на основе текущего.
			std::shared_ptr<demi::Species> newSpec = std::make_shared<demi::Species>(newTree->genotype, *oldSpec.get());

			// Проверяем, нет ли уже вида с подобными генами, иначе будет дублирование видов с одинаковыми мутациями.
			for (auto& derSpec : derSpecs) {
				// Если уже есть такой вид, возвращаем его.
				if (derSpec->isTheSameGeneValues(*newSpec.get()))
					return derSpec;
			}

			// Сохраняем новый вид с блокировкой от параллельного чтения в интерфейсном потоке.
			lockSpecies();
			derSpecs.push_back(newSpec);
			unlockSpecies();

			// Поднимаем флаг для внешнего кода, что есть изменения.
			flagSpaciesChanged = true;

			return newSpec;
		}

		// Возвращаем тот же самый вид, никаких мутаций нет.
		return oldSpec;
	}

	// Проверяем, нет ли уже вида с подобными генами, иначе будет дублирование видов с одинаковыми мутациями.
	for (auto& spec : species) {
		// Если уже есть такой вид, возвращаем его. Исходный вид для оптимизации не проверяем, он по-определению не совпадает.
		if (spec != oldSpec && spec->isTheSameGeneValues(*newGeneValues.get()))
			return spec;
	}

	// Создаём и сохраняем новый вид с блокировкой от параллельного чтения в интерфейсном потоке.
	std::shared_ptr<demi::Species> newSpec = std::make_shared<demi::Species>(*oldSpec.get(), newGeneValues);
	lockSpecies();
	species.push_back(newSpec);
	unlockSpecies();

	// Поднимаем флаг для внешнего кода, что есть изменения.
	flagSpaciesChanged = true;

	return newSpec;
}

// Удаляет вид.
void demi::GenotypesTree::removeSpecies(std::shared_ptr<demi::Species>& specToRemove)
{
	// Включаем блокировку.
	lockSpecies();
	
	// Находим интератор.
	auto it = std::find(species.begin(), species.end(), specToRemove);

	// Удаляем элемент.
	*it = std::move(species.back());
	species.pop_back();

	// Снимаем блокировку.
	unlockSpecies();

	// Флаг наличия изменений не поднимаем, чтобы пользователь мог увидеть 0. Обновление произойдёт при добавлении вида.
	//flagSpaciesChanged = true;
}

