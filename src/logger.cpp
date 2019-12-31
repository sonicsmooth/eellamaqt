
#include "logger.h"

#include <memory>

void Logger::setTextEdit(QTextEdit* pte) {
	m_pte = pte;
}

void Logger::log(const char *fmt, ...) {
	if (m_pte) {
		va_list args;
		va_start(args, fmt);
		size_t sz = static_cast<size_t>(vsnprintf(nullptr, 0, fmt, args)) + 1;
		std::unique_ptr<char> buff(new char [sz]);
		vsnprintf(buff.get(), sz, fmt, args);
		va_end(args);
		m_pte->append(buff.get());
	}
}
void Logger::log(std::string msg) {
	if (m_pte)
		m_pte->append(QString::fromStdString(msg));
}
void Logger::log(std::stringstream & msg) {
	// Clears stringstream when done
	if (m_pte)
		m_pte->append(QString::fromStdString(msg.str()));
	msg.str(std::string());
}
