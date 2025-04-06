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
        atomic<bool> flaga; // flag using in a reading loop
        atomic<bool> open, is_reading; // flag to check if port was open succesfully, and another flag to check if the reading is going on

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
        // end of singletona

        bool IsOpen() const;
        void Read();
        void StopReading();
        vector<string> GetLogs() const;
        ~Connector();
    };

    string ProcessLogs(const string& log);
}
