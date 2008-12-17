#include "Logger.h"
#include <fstream>

using namespace std;

void Logger::save_to( const char *file )
{
    ofstream fileStream;
    fileStream.open(file, ios_base::out);
    fileStream << buffer_;
    fileStream.close();
}

const char* Logger::c_str()
{
    return buffer_.c_str();
}

Logger::Logger()
{
    clear();
}

void Logger::clear()
{
    buffer_ = "";
}