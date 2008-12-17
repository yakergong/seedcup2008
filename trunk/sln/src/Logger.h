#pragma once

#include <sstream>
#include <string>

class Logger
{
public:
    Logger();
    template <typename T>
    Logger& operator <<(T value)
    {
        std::stringstream ss;
        ss << value;
        buffer_.append( ss.str() );
        return *this;
    }
    void save_to(const char *file);
    void clear();
    const char* c_str();
private:
    std::string buffer_;
};