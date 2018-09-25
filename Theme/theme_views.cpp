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
#include "theme_views.h"

namespace clan
{
	ThemeButtonView::ThemeButtonView()
	{
		style()->set("border-image-slice: 6 6 5 5 fill;");
		style()->set("border-image-width:6px 6px 5px 5px;");
		style()->set("border-image-repeat:stretch;");
		style()->set("border-image-source:url('Resources/button_normal.png');");
		style("hot")->set("border-image-source:url('Resources/button_hot.png');");
		style("pressed")->set("border-image-source:url('Resources/button_pressed.png');");
		style("disabled")->set("border-image-source:url('Resources/button_disabled.png');");
		label()->style()->set("margin: 5px auto; font: 13px/18px 'Segoe UI'; padding: 0 10px; color: rgb(0,0,0);");
		label()->style("disabled")->set("color: rgb(128,128,128);");
		label()->set_text_alignment(TextAlignment::center);
	}

	ThemeCheckBoxView::ThemeCheckBoxView()
	{
		style()->set("background-position:center center;");
		style()->set("background-repeat:no-repeat;");
		style()->set("background-attachment:scroll;");
		style()->set("width:13px; height:13px");
		style()->set("background-image:url('Resources/checkbox_unchecked_normal.png');");
		style("unchecked_hot")->set("background-image:url('Resources/checkbox_unchecked_hot.png');");
		style("unchecked_pressed")->set("background-image:url('Resources/checkbox_unchecked_pressed.png');");
		style("unchecked_disabled")->set("background-image:url('Resources/checkbox_unchecked_disabled.png');");
		style("checked")->set("background-image:url('Resources/checkbox_checked_normal.png');");
		style("checked_hot")->set("background-image:url('Resources/checkbox_checked_hot.png');");
		style("checked_pressed")->set("background-image:url('Resources/checkbox_checked_pressed.png');");
		style("checked_disabled")->set("background-image:url('Resources/checkbox_checked_disabled.png');");
	}

	ThemeImageView::ThemeImageView()
	{
	}

	ThemeLabelView::ThemeLabelView()
	{
		style()->set("font: 16px 'Segoe UI'; color: black");
	}

	ThemeListBoxView::ThemeListBoxView()
	{
		style()->set("margin: 7px 0; border: 1px solid black; padding: 5px; background: #f0f0f0");
	}

	ThemeListBoxLabelView::ThemeListBoxLabelView(const std::string &text)
	{
		style()->set("font: 13px/17px 'Segoe UI'; color: black; margin: 1px 0; padding: 0 2px");
		style("selected")->set("background: #7777f0; color: white;");
		style("hot")->set("background: #ccccf0; color: black");
		set_text(text);
	}

	ThemeRadioButtonView::ThemeRadioButtonView()
	{
		style()->set("background-position:center center;");
		style()->set("background-repeat:no-repeat;");
		style()->set("background-attachment:scroll;");
		style()->set("width:13px; height:13px");
		style()->set("background-image:url('Resources/radio_unchecked_normal.png');");
		style("unchecked_hot")->set("background-image:url('Resources/radio_unchecked_hot.png');");
		style("unchecked_pressed")->set("background-image:url('Resources/radio_unchecked_pressed.png');");
		style("unchecked_disabled")->set("background-image:url('Resources/radio_unchecked_disabled.png');");
		style("checked")->set("background-image:url('Resources/radio_checked_normal.png');");
		style("checked_hot")->set("background-image:url('Resources/radio_checked_hot.png');");
		style("checked_pressed")->set("background-image:url('Resources/radio_checked_pressed.png');");
		style("checked_disabled")->set("background-image:url('Resources/radio_checked_disabled.png');");
	}

	ThemeScrollView::ThemeScrollView()
	{
		scrollbar_x_view()->style()->set("flex: 0 0 main-size; background: rgb(232, 232, 236)");
		auto track = scrollbar_x_view()->track();
		track->style()->set("border-image-slice: 4 0 3 0 fill; border-image-width:4px 0px 3px 0px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_track_normal.png');");
		track->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_track_hot.png');");
		track->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_track_pressed.png');");
		track->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_track_disabled.png');");

		auto thumb = scrollbar_x_view()->thumb();
		thumb->style()->set("border-image-slice: 5 5 5 5 fill; border-image-width:5px 5px 5px 5px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_thumb_normal.png');");
		thumb->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_thumb_hot.png');");
		thumb->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_thumb_pressed.png');");
		thumb->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_thumb_disabled.png');");

		auto tg = scrollbar_x_view()->thumb_grip();
		tg->style()->set("background-position:center center; background-repeat:no-repeat; background-attachment:scroll; background-image:url('Resources/scrollbar_hori_thumb_gripper_normal.png');");
		tg->style("hot")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_hot.png');");
		tg->style("pressed")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_pressed.png');");
		tg->style("disabled")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_disabled.png');");

		auto bd = scrollbar_x_view()->button_decrement();
		bd->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_button_left_normal_withglyph.png');");
		bd->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_button_left_hot_withglyph.png');");
		bd->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_button_left_pressed_withglyph.png');");
		bd->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_button_left_disabled_withglyph.png');");

		auto bi = scrollbar_x_view()->button_increment();
		bi->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_button_right_normal_withglyph.png');");
		bi->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_button_right_hot_withglyph.png');");
		bi->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_button_right_pressed_withglyph.png');");
		bi->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_button_right_disabled_withglyph.png');");

		scrollbar_y_view()->style()->set("flex: 0 0 main-size; background: rgb(232, 232, 236)");
		track = scrollbar_y_view()->track();
		track->style()->set("border-image-slice: 4 0 3 0 fill; border-image-width:4px 0px 3px 0px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_vert_track_normal.png');");
		track->style("hot")->set("border-image-source:url('Resources/scrollbar_vert_track_hot.png');");
		track->style("pressed")->set("border-image-source:url('Resources/scrollbar_vert_track_pressed.png');");
		track->style("disabled")->set("border-image-source:url('Resources/scrollbar_vert_track_disabled.png');");

		thumb = scrollbar_y_view()->thumb();
		thumb->style()->set("border-image-slice: 5 5 5 5 fill; border-image-width:5px 5px 5px 5px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_vert_thumb_normal.png');");
		thumb->style("hot")->set("border-image-source:url('Resources/scrollbar_vert_thumb_hot.png');");
		thumb->style("pressed")->set("border-image-source:url('Resources/scrollbar_vert_thumb_pressed.png');");
		thumb->style("disabled")->set("border-image-source:url('Resources/scrollbar_vert_thumb_disabled.png');");

		tg = scrollbar_y_view()->thumb_grip();
		tg->style()->set("background-position:center center; background-repeat:no-repeat; background-attachment:scroll; background-image:url('Resources/scrollbar_vert_thumb_gripper_normal.png');");
		tg->style("hot")->set("background-image:url('Resources/scrollbar_vert_thumb_gripper_hot.png');");
		tg->style("pressed")->set("background-image:url('Resources/scrollbar_vert_thumb_gripper_pressed.png');");
		tg->style("disabled")->set("background-image:url('Resources/scrollbar_vert_thumb_gripper_disabled.png');");

		bd = scrollbar_y_view()->button_decrement();
		bd->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_vert_button_left_normal_withglyph.png');");
		bd->style("hot")->set("border-image-source:url('Resources/scrollbar_vert_button_left_hot_withglyph.png');");
		bd->style("pressed")->set("border-image-source:url('Resources/scrollbar_vert_button_left_pressed_withglyph.png');");
		bd->style("disabled")->set("border-image-source:url('Resources/scrollbar_vert_button_left_disabled_withglyph.png');");

		bi = scrollbar_y_view()->button_increment();
		bi->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_vert_button_right_normal_withglyph.png');");
		bi->style("hot")->set("border-image-source:url('Resources/scrollbar_vert_button_right_hot_withglyph.png');");
		bi->style("pressed")->set("border-image-source:url('Resources/scrollbar_vert_button_right_pressed_withglyph.png');");
		bi->style("disabled")->set("border-image-source:url('Resources/scrollbar_vert_button_right_disabled_withglyph.png');");
	}

	ThemeScrollBarView::ThemeScrollBarView()
	{
		set_horizontal();
		style()->set("flex: 0 0 main-size; background: rgb(232, 232, 236)");
		track()->style()->set("border-image-slice: 4 0 3 0 fill; border-image-width:4px 0px 3px 0px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_track_normal.png');");
		track()->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_track_hot.png');");
		track()->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_track_pressed.png');");
		track()->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_track_disabled.png');");

		thumb()->style()->set("border-image-slice: 5 5 5 5 fill; border-image-width:5px 5px 5px 5px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_thumb_normal.png');");
		thumb()->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_thumb_hot.png');");
		thumb()->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_thumb_pressed.png');");
		thumb()->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_thumb_disabled.png');");

		thumb_grip()->style()->set("background-position:center center; background-repeat:no-repeat; background-attachment:scroll; background-image:url('Resources/scrollbar_hori_thumb_gripper_normal.png');");
		thumb_grip()->style("hot")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_hot.png');");
		thumb_grip()->style("pressed")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_pressed.png');");
		thumb_grip()->style("disabled")->set("background-image:url('Resources/scrollbar_hori_thumb_gripper_disabled.png');");

		button_decrement()->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_button_left_normal_withglyph.png');");
		button_decrement()->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_button_left_hot_withglyph.png');");
		button_decrement()->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_button_left_pressed_withglyph.png');");
		button_decrement()->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_button_left_disabled_withglyph.png');");

		button_increment()->style()->set("width:17px; height:17px; border-image-slice: 3 3 3 3 fill; border-image-width:3px 3px 3px 3px; border-image-repeat:stretch; border-image-source:url('Resources/scrollbar_hori_button_right_normal_withglyph.png');");
		button_increment()->style("hot")->set("border-image-source:url('Resources/scrollbar_hori_button_right_hot_withglyph.png');");
		button_increment()->style("pressed")->set("border-image-source:url('Resources/scrollbar_hori_button_right_pressed_withglyph.png');");
		button_increment()->style("disabled")->set("border-image-source:url('Resources/scrollbar_hori_button_right_disabled_withglyph.png');");
	}

	ThemeSliderView::ThemeSliderView()
	{
		set_horizontal();

		style()->set("flex-direction: row;");
		track()->style()->set("flex: 1 1 main-size; height: 4px; margin: 7px 0px; border-image-slice: 1 2 1 1 fill; border-image-width:1px 2px 1px 1px; border-image-repeat:stretch; border-image-source:url('Resources/slider_track.png');");
		thumb()->style()->set("position: absolute; width:11px; height:19px; border-image-slice:9 3 9 2 fill; border-image-width:9px 3px 9px 2px; border-image-repeat:stretch; border-image-source:url('Resources/slider_horizontal_thumb_normal.png');");
		thumb()->style("hot")->set("border-image-source:url('Resources/slider_horizontal_thumb_hot.png');");
		thumb()->style("pressed")->set("border-image-source:url('Resources/slider_horizontal_thumb_pressed.png');");
		thumb()->style("disabled")->set("border-image-source:url('Resources/slider_horizontal_thumb_disabled.png');");
	}

	ThemeSpinView::ThemeSpinView()
	{
	}

	ThemeTextFieldView::ThemeTextFieldView()
	{
	}

	ThemeTextView::ThemeTextView()
	{
	}
}
