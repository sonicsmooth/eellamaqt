#include "logger.h"


void Logger::setTextEdit(QTextEdit* pte) {
	m_pte = pte;
}

void Logger::log(const char* msg) {
	if (m_pte)
		m_pte->append(msg);;
}

void Logger::log(std::string msg) {
	if (m_pte)
		m_pte->append(QString::fromStdString(msg));
}