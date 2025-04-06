#pragma once
#include <wx/wx.h>

namespace USB_reader {
	class Connector;
};

enum IDs{
	BUTTON_ID = 2,
	TEXT_ID = 3,
	START_READING = 4,
	STOP_READING = 5,
	REREAD = 6,
	SHOW_DATA = 7,
	SHOW_DATA_CALL = 8,
};


class MainFrame : public wxFrame{
private:
	USB_reader::Connector* reader; 

	// GUI
	wxPanel* panel;

	wxButton* startReading;
	wxButton* stopReading;
	wxButton* reRead;
	wxButton* showLogs;
	wxButton* portInputButton; 

	wxTextCtrl* portNameInput;

	wxListBox* logsBox;

	wxString portName;

	//methods
	void PortInput(wxCommandEvent& event);
	void BeginRead(wxCommandEvent& event);
	void StopRead(wxCommandEvent& event);
	void ReRead(wxCommandEvent& event);
	void UpdateGUI(wxCommandEvent& event);
	void DisplayInfo(wxCommandEvent& event);

public:
	MainFrame(const wxString& title); 
	~MainFrame();

	wxDECLARE_EVENT_TABLE();
};

wxDECLARE_EVENT(wxEVT_UPDATE_GUI, wxCommandEvent); // table that synchronizes events
