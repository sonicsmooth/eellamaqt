#ifndef LIBTREEWIDGET_H
#define LIBTREEWIDGET_H

#include <QDockWidget>
#include <QTreeView>
#include <QPushButton>

#include "libcore.h"


class LibTreeWidget : public QWidget
{
private:
    LibCore *m_pCore;
    std::string m_dbConnName;
    QTreeView* m_pQTreeView;
    QPushButton* m_pPBNewShape;
    QPushButton* m_pPBNewSymbol;
    QPushButton* m_pPBDelete;

public:
    LibTreeWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    void setDbConnName(std::string);
    std::string DbConnName() const;
    void setCore(LibCore*);
    QPushButton* PBShape() const;
    QPushButton* PBSymbol() const;
    QPushButton* PBDelete() const;

};

#endif // LIBTREEWIDGET_H
