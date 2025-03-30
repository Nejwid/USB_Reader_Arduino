#include "MainFrame.h"
#include "USB_reader.h"

void MainFrame::PortInput(wxCommandEvent& event) { // funkcja do poprawnego zainicjowania instancji Connector
	portName = portNameInput->GetValue();
	USB_reader::Connector::Reset();    
	reader = &USB_reader::Connector::getInstance(string(portName.mb_str()));
	if (reader->IsOpen()) {
		portNameInput->Hide();
		portInputButton->Hide();
		wxLogStatus("done");
		wxCommandEvent updateEvent(wxEVT_UPDATE_GUI);
		updateEvent.SetString("show_reading_buttons");
		wxPostEvent(this, updateEvent);
	}
	else {
		wxLogStatus("invalid port or no device connected");
		portNameInput->Show();
		portInputButton->Show();
	}
}

void MainFrame::BeginRead(wxCommandEvent& event) {
	reader->Read(); 
	wxLogStatus("reading"); 

	wxCommandEvent updateEvent(wxEVT_UPDATE_GUI); 
	updateEvent.SetString("begin_reading");
	wxPostEvent(this, updateEvent); 
}

// po zatrzymaniu odczytywania trzeba jeszcze jeden bufor odebrac bo tak dziala funkcja read - blokuje wszystko dopóki nie dotanie bufora, dlatego wysy³amy komunikat ¿e chcemy zakoñczyæ i arduino nam odpowiada
void MainFrame::StopRead(wxCommandEvent& event) {
	reader->StopReading(); 
	wxLogStatus("stopped"); 

	wxCommandEvent updateEvent(wxEVT_UPDATE_GUI); 
	updateEvent.SetString("stop_reading");
	wxPostEvent(this, updateEvent);
}

void MainFrame::ReRead(wxCommandEvent& event) {
	reader->Read(); 
	wxLogStatus("re-reading");

	wxCommandEvent updateEvent(wxEVT_UPDATE_GUI);
	updateEvent.SetString("re_reading");
	wxPostEvent(this, updateEvent);
}

void MainFrame::DisplayInfo(wxCommandEvent& event) {
	vector<string> temp = reader->GetLogs();
	logsBox->Clear();
	for (const string& it : temp) { // zawartosc jest already przetworzona
		logsBox->Append(it);
	}
	wxLogStatus("showing data");

	wxCommandEvent updateEvent(wxEVT_UPDATE_GUI);
	updateEvent.SetString("showing_data");
	wxPostEvent(this, updateEvent);
}

void MainFrame::UpdateGUI(wxCommandEvent& event) {
	wxString action = event.GetString();

	if (action == "show_reading_buttons") {
		portNameInput->Hide();
		portInputButton->Hide();
		startReading->Show();
		Refresh();
	} else if (action == "begin_reading") {
		startReading->Hide();
		stopReading->Show();
	} else if (action == "stop_reading") {
		showLogs->Show();
		stopReading->Hide();
		reRead->Show();
	} else if (action == "re_reading") {
		logsBox->Hide();
		showLogs->Hide();
		reRead->Hide();
		stopReading->Show();
		Refresh();
	} else if (action == "showing_data") {
		logsBox->Show();
	}
}

MainFrame::MainFrame(const wxString& title):wxFrame(nullptr, wxID_ANY, title), reader(nullptr) {
	CreateStatusBar(1); // pasek na dole (liczba sekcji)
	panel = new wxPanel(this, wxID_ANY);
	portInputButton = new wxButton(panel, BUTTON_ID, "zatwierdz port szeregowy", wxPoint(200, 355), wxSize(200, 40));
	portNameInput = new wxTextCtrl(panel, TEXT_ID, "", wxPoint(200, 305), wxSize(200, 40));
	startReading = new wxButton(panel, START_READING, "begin reading", wxPoint(200, 50), wxSize(200, 40));
	startReading->Hide();
	stopReading = new wxButton(panel, STOP_READING, "stop reading", wxPoint(400, 50), wxSize(200, 40));
	stopReading->Hide();
	reRead = new wxButton(panel, REREAD, "begin reading", wxPoint(600, 50), wxSize(200, 40));
	reRead->Hide();
	showLogs = new wxButton(panel, SHOW_DATA_CALL, "show logs", wxPoint(200, 355), wxSize(200, 40));
	showLogs->Hide();
	logsBox = new wxListBox(panel, SHOW_DATA, wxPoint(440, 150), wxSize(400, 400));
	logsBox->Hide();
}

MainFrame::~MainFrame(){}

//-----------------------------------------------------------------------------------------------------------------------------------//

wxDEFINE_EVENT(wxEVT_UPDATE_GUI, wxCommandEvent);

// to tez na koncu poza klas¹ jak pole statyczne, deklaracja w .h, tutaj begin-end
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(BUTTON_ID, MainFrame::PortInput)
EVT_BUTTON(START_READING, MainFrame::BeginRead)
EVT_BUTTON(STOP_READING, MainFrame::StopRead)
EVT_BUTTON(REREAD, MainFrame::ReRead)
EVT_BUTTON(SHOW_DATA_CALL, MainFrame::DisplayInfo)
EVT_COMMAND(wxID_ANY, wxEVT_UPDATE_GUI, MainFrame::UpdateGUI)
wxEND_EVENT_TABLE()