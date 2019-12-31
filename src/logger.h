#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <QTextEdit>

#include "ilogger.h"

class Logger : public ILogger {
private:
	QTextEdit* m_pte;
public:
	Logger() = default;
	Logger(QTextEdit* pte) : m_pte(pte) {}
	~Logger() override = default;
	QTextEdit* textEdit() { return m_pte; }
	void setTextEdit(QTextEdit*);
	void log(const char *, ...) override; // Use like printf
	void log(std::string) override;
	void log(std::stringstream &) override; // clears string when done
};

#endif