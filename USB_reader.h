#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <atomic>

using namespace std;

namespace USB_reader {

    class Connector {
    private:
        boost::asio::io_service usb;
        boost::asio::serial_port port;
        boost::asio::streambuf buffer;
        thread t1; 
        vector<string> logs;
        atomic<bool> flaga; // flaga do pêtli odczytu danych
        atomic<bool> open, is_reading; // flaga do sprawdzenia czy port zosta³ otwarty i czy trwa odczytywanie danych

        void StopReadingData();
        void ReadData();
        void DisplayInfo() const;

        // singleton part:
        Connector(const string& com);
        static unique_ptr<Connector> instance;
        static mutex mtx;

    public:
        Connector(const Connector&) = delete;
        Connector operator= (const Connector&) = delete;
        static Connector& getInstance(const string& com);
        static void Reset() { instance.reset(); };
        // koniec singletona

        bool IsOpen() const;
        void Read();
        void StopReading();
        vector<string> GetLogs() const;
        ~Connector();
    };

    string ProcessLogs(const string& log);
}