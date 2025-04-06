#pragma once
#include <wx/wx.h>
#include "MainFrame.h"

class App : public wxApp{
private:

public:
	bool OnInit(){
		MainFrame* frame = new MainFrame("USB Input Reader by Dawid Nowak");
		frame->Show();
		return true;
	}
};
