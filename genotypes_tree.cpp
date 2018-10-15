/* ===============================================================================
������������� �������� ������ ����.
����������� �������� ��������� � ����� ��� �������������� � �������������.
10 october 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "organism.h"
#include "gene.h"
#include "genotypes_tree.h"



// ������ ������� ��� ������������� ����� � ��������.
void GenotypesTree::generateDict(speciesDict_t& dict)
{
	// �������� ����������� ����.
	for (const std::shared_ptr<demi::Species> spec : species)
		dict.push_back(spec);

	// �������� ���������� ��� ������� ������������ ����.
	for (auto derivative : derivatives)
		derivative->generateDict(dict);
}

// ���������� ������ �� ����.
void GenotypesTree::saveToFile(clan::File& binFile)
{
	// ���������� ����������� �����.
	uint32_t cnt = uint32_t(species.capacity());
	binFile.write_uint32(cnt);
	
	// ���� ����������� ����.
	for (const std::shared_ptr<demi::Species> spec : species)
		spec->saveToFile(binFile);

	// ����������� ���� (�� ���������� �������������� �� XML).
	for (auto derivative : derivatives)
		derivative->saveToFile(binFile);
}

// ��������� ���� � �����.
void GenotypesTree::loadFromFile(clan::File& binFile)
{
	// ���������� ����������� �����.
	uint32_t cnt = binFile.read_uint32();

	// ���� ����������� ����.
	for (uint32_t i = 0; i != cnt; ++i)
		species.push_back(std::make_shared<demi::Species>(genotype, binFile));

	// ����������� ���� (�� ���������� �������������� �� XML).
	for (auto derivative : derivatives)
		derivative->loadFromFile(binFile);
}
