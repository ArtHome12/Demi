/* ===============================================================================
������������� �������� ������ ����.
�������������������� �������.
26 september 2018.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2016 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

#include "precomp.h"
#include "Theme/theme.h"
#include "msg_boxes.h"

// ��������� �������
auto cMsgBoxOk = "MsgBoxOk";
auto cMsgBoxCancel = "MsgBoxCancel";


MsgBox::MsgBox(SettingsStorage* pSettings, const std::string& text, const std::string& caption, eMbType mbType) :
	leftIcon(std::make_shared<clan::ImageView>()),
	bOk(Theme::create_button()),
	bCancel(Theme::create_button())
{
	auto rootView = root_view();

	set_title(caption);
	rootView->style()->set("flex-direction: row; background: rgb(240,240,240); padding: 11px; width: 250px");

	// ������ � ����� ����� ������� (�������� ������ ���� ��� ����� loadIcons().
	rootView->add_child(leftIcon);

	// ������ ������ �������� ����, ����� ��������� � �������� �������.
	// ������� ������� ��� �����������.
	int primaryScreenIndex = 0;
	std::vector<clan::Rectf> screenRects = clan::ScreenInfo().get_screen_geometries(primaryScreenIndex);
	int screenWidth = int(screenRects.at(size_t(primaryScreenIndex)).get_width());

	// ������ ������� �� ������ ���� ����� 300 ����� � ����� �������� ������.
	screenWidth = std::max<int>(300, screenWidth / 4); //-V112

	// ����� ��� ������ � ������ ������ ������.
	auto rightPanel = std::make_shared<clan::View>();
	rightPanel->style()->set("flex-direction: column; width: " + std::to_string(screenWidth) + "px");
	rightPanel->style()->set("border: 1px solid red");
	rootView->add_child(rightPanel);

	// �����.
	auto span = Theme::create_span();
	span->add_text(text);
	rightPanel->add_child(span);

	// �������� ������ ����� � ��������.
	auto rightBottomPanel = std::make_shared<clan::View>();
	rightBottomPanel->style()->set("flex-direction: row; height: 32px;");
	rightBottomPanel->style()->set("border: 1px solid green");
	rightPanel->add_child(rightBottomPanel);

	bOk->label()->set_text(pSettings->LocaleStr(cMsgBoxOk));
	rightPanel->add_child(bOk);

	bOk->func_clicked() = [=]()
	{
		result = cMbResultOk;
		dismiss();
	};
}

MsgBox::~MsgBox()
{
	// �������� ����������. ���������� �� �����������, ��� ��� slots.connect(sig_close()) �� ��������.
	if (onProcessResult)
		onProcessResult(result);
}

void MsgBox::loadIcons(clan::Canvas &canvas, const clan::FileSystem& fs)
{
	leftIcon->set_image(clan::Image(canvas, "Exclamation.png", fs));
	leftIcon->style()->set("border: 1px solid red");

	bOk->image_view()->set_image(clan::Image(canvas, "Ok.png", fs));
}
