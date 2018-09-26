/* ===============================================================================
Моделирование эволюции живого мира.
Платформонезависимые диалоги.
26 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "Theme/theme.h"
#include "msg_boxes.h"

MsgBox::MsgBox(const std::string& text, const std::string& caption, eMbType mbType)
{
	set_title(caption);
	root_view()->style()->set("flex-direction: column; background: rgb(240,240,240); padding: 11px; width: 250px");

	auto label = Theme::create_label(true);
	label->style()->set("margin-bottom: 7px; font: 12px Tahoma; color: black");
	label->set_text(text);
	root_view()->add_child(label);

	auto ok_button = Theme::create_button();
	ok_button->label()->set_text("OK");
	root_view()->add_child(ok_button);

	ok_button->func_clicked() = [=]()
	{
		result = cMbResultOk;
		dismiss();
	};
}

MsgBox::~MsgBox()
{
	// Вызываем обработчик. Приходится из деструктора, так как slots.connect(sig_close()) не работает.
	if (onProcessResult)
		onProcessResult(result);
}

