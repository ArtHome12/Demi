/*
**  ClanLib SDK
**  Copyright (c) 1997-2016 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    Mark Page
*/

#include "..\precomp.h"
#include "theme.h"

using namespace clan;

std::shared_ptr<clan::ScrollBarView> Theme::create_scrollbar()
{
	auto scrollbar = std::make_shared<clan::ScrollBarView>(false);
	scrollbar->set_horizontal();
	scrollbar->style()->set("flex: 0 0 auto; background: rgb(232, 232, 236);");
	auto track = scrollbar->track();
	track->style()->set("border-image-slice: 4 0 3 0 fill; border-image-width:4px 0px 3px 0px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_track_normal.png');");
	track->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_track_hot.png');");
	track->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_track_pressed.png');");
	track->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_track_disabled.png');");

	auto thumb = scrollbar->thumb();
	thumb->style()->set("border-image-slice: 5 5 5 5 fill; border-image-width:5px 5px 5px 5px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_thumb_normal.png');");
	thumb->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_thumb_hot.png');");
	thumb->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_thumb_pressed.png');");
	thumb->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_thumb_disabled.png');");

	auto tg = scrollbar->thumb_grip();
	tg->style()->set("background-position:center center; background-repeat:no-repeat; background-attachment:scroll; background-image:url('Resources/scrollbar_hori_thumb_gripper_normal.png');");
	tg->style("hot")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_hot.png');");
	tg->style("pressed")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_pressed.png');");
	tg->style("disabled")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_disabled.png');");

	auto bd = scrollbar->button_decrement();
	bd->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_button_left_normal_withglyph.png');");
	bd->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_button_left_hot_withglyph.png');");
	bd->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_button_left_pressed_withglyph.png');");
	bd->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_button_left_disabled_withglyph.png');");

	auto bi = scrollbar->button_increment();
	bi->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_button_right_normal_withglyph.png');");
	bi->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_button_right_hot_withglyph.png');");
	bi->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_button_right_pressed_withglyph.png');");
	bi->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_button_right_disabled_withglyph.png');");
	return scrollbar;
}

std::shared_ptr<clan::ButtonView> Theme::create_button()
{
	auto button = std::make_shared<clan::ButtonView>();
	button->style()->set("border-image-slice: 6 6 5 5 fill; border-image-width:6px 6px 5px 5px; border-image-repeat:stretch; border-image-source:url('Resources/button_normal.png');");
	button->style("hot")->set("border-image-source:url('Resources/button_hot.png');");
	button->style("pressed")->set("border-image-source:url('Resources/button_pressed.png');");
	button->style("pressed_hot")->set("border-image-source:url('Resources/button_pressed_hot.png');");
	button->style("disabled")->set("border-image-source:url('Resources/button_disabled.png');");
	auto label = button->label();
	label->style()->set("font: 13px/1.5 'Segoe UI'; padding: 3px 10px; color: rgb(0,0,0);");
	label->style("disabled")->set("color: rgb(128,128,128);");
	label->style("pressed_hot")->set("font-weight: bold;");
	label->set_text_alignment(TextAlignment::center);
	return button;
}

std::shared_ptr<clan::SliderView> Theme::create_slider()
{
	std::shared_ptr<clan::SliderView> slider = std::make_shared<clan::SliderView>();
	slider->set_horizontal();

	slider->style()->set("flex-direction: row;");
	slider->track()->style()->set("flex: 1 1 auto; height: 4px; margin: 7px 0px; border-image-slice: 1 2 1 1 fill; border-image-width:1px 2px 1px 1px; border-image-repeat:stretch; border-image-source:url('Resources/slider_track.png');");
	auto thumb = slider->thumb();
	thumb->style()->set("position: absolute; width:11px; height:19px; border-image-slice:9 3 9 2 fill; border-image-width:9px 3px 9px 2px; border-image-repeat:stretch; border-image-source:url('Resources/slider_horizontal_thumb_normal.png');");
	thumb->style("hot")->set("border-image-source:url('Resources/slider_horizontal_thumb_hot.png');");
	thumb->style("pressed")->set("border-image-source:url('Resources/slider_horizontal_thumb_pressed.png');");
	thumb->style("disabled")->set("border-image-source:url('Resources/slider_horizontal_thumb_disabled.png');");

	return slider;
}

std::shared_ptr<clan::CheckBoxView> Theme::create_checkbox()
{
	auto checkbox = std::make_shared<clan::CheckBoxView>();

	checkbox->style()->set("background-repeat:no-repeat; background-attachment:scroll; background-position: center left; background-image:url('Resources/checkbox_unchecked_normal.png');");
	checkbox->style("unchecked_hot")->set("background-image:url('Resources/checkbox_unchecked_hot.png');");
	checkbox->style("unchecked_pressed")->set("background-image:url('Resources/checkbox_unchecked_pressed.png');");
	checkbox->style("unchecked_disabled")->set("background-image:url('Resources/checkbox_unchecked_disabled.png');");
	checkbox->style("checked")->set("background-image:url('Resources/checkbox_checked_normal.png');");
	checkbox->style("checked_hot")->set("background-image:url('Resources/checkbox_checked_hot.png');");
	checkbox->style("checked_pressed")->set("background-image:url('Resources/checkbox_checked_pressed.png');");
	checkbox->style("checked_disabled")->set("background-image:url('Resources/checkbox_checked_disabled.png');");
	checkbox->label()->style()->set("font: 13px 'Segoe UI'; padding: 0px 30px; color: black;");
	auto label = checkbox->label();
	label->style("disabled")->set("color: rgb(128,128,128);");
	label->set_text_alignment(TextAlignment::left);
	return checkbox;
}

std::shared_ptr<clan::RadioButtonView> Theme::create_radiobutton()
{
	auto radio = std::make_shared<clan::RadioButtonView>();
	radio->style()->set("background-position:center center; background-repeat:no-repeat; background-attachment:scroll; width:13px; height:13px; background-image:url('Resources/radio_unchecked_normal.png');");
	radio->style("unchecked_hot")->set("background-image:url('Resources/radio_unchecked_hot.png');");
	radio->style("unchecked_pressed")->set("background-image:url('Resources/radio_unchecked_pressed.png');");
	radio->style("unchecked_disabled")->set("background-image:url('Resources/radio_unchecked_disabled.png');");
	radio->style("checked")->set("background-image:url('Resources/radio_checked_normal.png');");
	radio->style("checked_hot")->set("background-image:url('Resources/radio_checked_hot.png');");
	radio->style("checked_pressed")->set("background-image:url('Resources/radio_checked_pressed.png');");
	radio->style("checked_disabled")->set("background-image:url('Resources/radio_checked_disabled.png');");
	return radio;
}

std::shared_ptr<clan::LabelView> Theme::create_label(bool subpixel)
{
	auto label = std::make_shared<clan::LabelView>();
	label->style()->set("font: 12px Tahoma; color: white");
	if (!subpixel)
		label->style()->set("-clan-font-rendering: anti-alias;");
	return label;
}

std::shared_ptr<clan::SpanLayoutView> Theme::create_span()
{
	auto span = std::make_shared<SpanLayoutView>();
	span->style()->set("font: 12px Tahoma");
	return span;
}

std::shared_ptr<clan::ListBoxView> Theme::create_listbox()
{
	auto listbox = std::make_shared<clan::ListBoxView>();
	listbox->style()->set("margin: 7px 0; border: 1px solid black; padding: 5px; background: #f0f0f0");
	return listbox;
}

std::shared_ptr<clan::LabelView> Theme::create_listbox_label(const std::string &text)
{
	auto label = std::make_shared<clan::LabelView>();
	label->style()->set("font: 13px/17px 'Segoe UI'; color: black; margin: 1px 0; padding: 0 2px");
	label->style("selected")->set("background: #7777f0; color: white;");
	label->style("hot")->set("background: #ccccf0; color: black");
	label->set_text(text);
	return label;
}
