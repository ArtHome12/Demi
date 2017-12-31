/* ===============================================================================
Моделирование эволюции живого мира.
Модуль для химических реакций.
31 december 2017.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2018 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */
#pragma once



namespace demi {

class ReactionReagent {
public:
	int elementIndex, amount;
	ReactionReagent(int _elementIndex, int _amount) : elementIndex(_elementIndex), amount(_amount) {};
};

class ChemReaction {
public:
	// Название - на всякий случай.
	std::string name;

	// Количество необходимой геотермальной и солнечной энергии.
	float geoEnergy, solarEnergy;

	// Реагенты (индексы элементов неживой природы) с левой и с правой сторон.
	std::forward_list<ReactionReagent> leftReagents, rightReagents;
};

};