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


// ���� ���������� ��� ����� ���������, ���� ���, �� ���������, ���� ����, �� ���������� ������ �� ������������.
const std::shared_ptr<demi::Species>& GenotypesTree::findSpecies(const std::shared_ptr<demi::Species>& speciesToFindSimilar) 
{ 
	return species.front(); 
}

