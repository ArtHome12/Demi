/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для подсчёта количества элементов и организмов с целью отображения.
12 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#pragma once

class Amounts {

public:

	~Amounts() { delete[] arResAmounts; }

	// Инициализация класса. Его инициализация должна происходить после инициализации массивов в World, когда модель готова к первому тику.
	void Init();

	unsigned long long getResAmounts(size_t index) { return arResAmounts ? arResAmounts[index] : 0; }

	// Увеличивает количество указанного элемента. Можно увеличивать на отрицательную величину.
	void incAmount(size_t index, unsigned long long amnt) { arResAmounts[index] += amnt; }
	void decAmount(size_t index, unsigned long long amnt) { arResAmounts[index] -= amnt; }

private:

	// Массив количеств элементов неживой природы. 
	unsigned long long *arResAmounts = nullptr;

};