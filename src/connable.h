#ifndef ICONNABLE_H
#define ICONNABLE_H

#include <string>

class Connable {
    public:
        std::string m_dbConn;
        Connable(std::string s = std::string()) : m_dbConn(s) {}
        void setDbConn(std::string c) {m_dbConn = c;}
        std::string dbConn() const {return m_dbConn;}
        std::string abbrev() const;
};


#endif
