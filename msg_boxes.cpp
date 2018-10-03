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
	rootView->style()->set("flex-direction: row; background: rgb(240,240,240); padding: 11px");

	// ������ � ����� ����� ������� (�������� ������ ���� ��� ����� loadIcons().
	leftIcon->style()->set("padding-right: 11px");
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
	//rightPanel->style()->set("border: 1px solid red");
	rootView->add_child(rightPanel);

	// �����.
	auto span = Theme::create_span();
	span->style()->set("padding: 11px; margin-bottom: 11px");
	span->add_text(text);
	rightPanel->add_child(span);

	// �������� ������ ����� � ��������, �� ������ ���� ������ ����.
	auto rightBottomPanel = std::make_shared<clan::View>();
	if (mbType != cMbNone) {
		rightBottomPanel->style()->set("flex-direction: row; height: 32px; justify-content: center");
		//rightBottomPanel->style()->set("border: 1px solid green");
		rightPanel->add_child(rightBottomPanel);
	}

	if (mbType == cMbOkCancel || mbType == cMbOk) {
		bOk->style()->set("width: 120px; margin-right: 30px");
		bOk->image_view()->style()->set("padding-left: 3px");
		bOk->label()->set_text(pSettings->LocaleStr(cMsgBoxOk));
		rightBottomPanel->add_child(bOk);

		bOk->func_clicked() = [=]()
		{
			result = cMbResultOk;
			dismiss();
		};
	}
	else
		// ��� ����������� ����������.
		bOk->set_hidden();

	if (mbType == cMbOkCancel || mbType == cMbCancel) {
		bCancel->style()->set("width: 120px");
		bCancel->image_view()->style()->set("padding-left: 3px");
		bCancel->label()->set_text(pSettings->LocaleStr(cMsgBoxCancel));
		rightBottomPanel->add_child(bCancel);

		bCancel->func_clicked() = [=]()
		{
			dismiss();
		};
	}

	// ����� ����������� ������ ������� ����������� ����.
	set_resizable(false);
}

MsgBox::~MsgBox()
{
	// �������� ���������� ��� ����������� � ������ ������������.
	if (onProcessResult)
		onProcessResult(result);
}

void MsgBox::initWindow(const clan::FileSystem& fs)
{
	auto& rootView = root_view();
	auto& canvas = rootView->canvas();

	rootView->slots.connect(rootView->sig_key_press(), [&](clan::KeyEvent &e) { on_input_down(e); });
	// ��� ������ ���� ������� ������� ���������� �� ��������.
	rootView->set_focus();

	// ��������� ������.
	leftIcon->set_image(clan::Image(canvas, "Exclamation.png", fs));
	bOk->image_view()->set_image(clan::Image(canvas, "Ok.png", fs));
	bCancel->image_view()->set_image(clan::Image(canvas, "Cancel.png", fs));
}


void MsgBox::on_input_down(const clan::KeyEvent &e)
{
	switch (e.key())
	{
	case clan::Key::escape:
		dismiss();
		break;
	case clan::Key::key_return:
		if (!bOk->hidden())
			result = cMbResultOk;
		dismiss();
		break;
	}
}
