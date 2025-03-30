#include "USB_reader.h"

namespace USB_reader {

    unique_ptr<Connector> Connector::instance;
    mutex Connector::mtx;

    Connector::Connector(const string& com) : port(usb), flaga(true), open(false), is_reading(false) {
        try {
            port.open(com);
            port.set_option(boost::asio::serial_port_base::baud_rate(9600)); // dopasowanie z baud rate ustawionym w Arduino
            open = true;
        }
        catch (const std::exception& e) {
            std::cerr << "NO ARDUINO FOUND CONNECTED, connect arduino and reboot the app. Note that the device must be connected before running this app. Error opening port : " << com << endl;
        }
    }

    Connector& Connector::getInstance(const string& com) { // funkcja singletonowa
        lock_guard<mutex> lock(mtx);
        if (!instance)
            instance.reset(new Connector(com));
        return *instance;
    }

    void Connector::ReadData() {
        string startReading = "stop"; 
        boost::asio::write(this->port, boost::asio::buffer(startReading)); // informujemy arduino zeby zacz�� serial.println, odpowiednia funckja zaimplementowana w sofcie kontrolera
        while (this->flaga) {
            boost::asio::read(port, buffer, boost::asio::transfer_at_least(1)); // bez tego bufora tez bedzie dzialac bo ta funkcja odczytuje jeden raz write, wi�c mozna stworzyc bufor in place
            while (buffer.size() > 0) {
                string temp = "";

                auto begin = boost::asio::buffers_begin(buffer.data());
                auto end = boost::asio::buffers_end(buffer.data());

                for (auto it = begin; it != end; it++) {
                    if (*it == '\n') break;
                    if(*it != ' ') temp += *it;
                }

                temp = ProcessLogs(temp);
                if(!temp.empty()) logs.push_back(temp);
                buffer.consume(end - begin);
            }
        }
    }

    void Connector::StopReadingData() {
        string endReading = "stop";
        boost::asio::write(this->port, boost::asio::buffer(endReading)); // ta funkcja informuje arduino �e ju� nie odczytujemy, a funkcja boost::asio::read otrzyma od arduino informacje o ko�cu nadawania
        this->flaga = false;
    }

    bool Connector::IsOpen() const { // sprawdzamy czy instancja zosta�a utworzona zgodnie z zamierzeniem tj. czy uzyska�a dost�p do portu szeregowego
        return this->open;
    }

    void Connector::DisplayInfo() const {
        if (!logs.empty()) {
            lock_guard<mutex> lock(mtx);
            for (const auto& it : logs) {
                cout << it << endl;
            }
        }
        else cout << "no data found. read data first" << endl;
    }

    void Connector::Read() { // rozpocz�cie czytania danych
        if (is_reading) {
            return;
        }

        is_reading = true;
        this->flaga = true;
        if (!t1.joinable()) {
            t1 = thread([this] {
                this->ReadData();
                });
        }
    }

    void Connector::StopReading() { // zako�czenie (przerwanie) czytania danych
        if (!is_reading) {
            return;
        }

        this->StopReadingData();

        if (t1.joinable()) {
            t1.join();
        }

        is_reading = false;
    }

    // metoda do pobierania log�w
    vector<string> Connector::GetLogs() const {
        lock_guard<mutex> lock(mtx);
        return logs;
    }

    Connector::~Connector() {}

    string ProcessLogs(const string& log) { // funkcja usuwania spacji tabulator�w i bia�ych znak�w
        string trimmed = log;
        size_t firstNonWhitespace = trimmed.find_first_not_of(" \t\n\r");
        size_t lastNonWhitespace = trimmed.find_last_not_of(" \t\n\r");

        // Je�eli nie ma �adnych znak�w poza bia�ymi, trimmed b�dzie pusty
        if (firstNonWhitespace != string::npos && lastNonWhitespace != string::npos) {
            trimmed = trimmed.substr(firstNonWhitespace, lastNonWhitespace - firstNonWhitespace + 1);
        }
        else {
            trimmed.clear();  // Je�eli ca�y ci�g to bia�e znaki, zr�b go pustym
        }
        return trimmed;
    }
}