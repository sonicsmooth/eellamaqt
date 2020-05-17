#ifndef LOGGER_H
#define LOGGER_H

#include "ilogger.h"

#include <QObject>
#include <QTextEdit>
#include <QString>


#include <iostream>

class Logger : public QObject, public ILogger {
private:
	QTextEdit* m_pte; // Logger does not own textedit
public:
	Logger() = default;
	Logger(QTextEdit* pte) : m_pte(pte) {}
	~Logger() override;;
	QTextEdit* textEdit() { return m_pte; }
	void setTextEdit(QTextEdit*);
    void vlog(const char *, va_list) override;
	void log(const char *, ...) override; // Use like printf
	void log(std::string) override;
    void log(std::stringstream &) override; // clears string when done
    void log(QString);
public slots:
	void nullifyLogger();
};

#endif
