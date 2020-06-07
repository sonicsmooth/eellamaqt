#include "libsymbolview.h"
#include "docwindow.h"

#include <QWidget>
#include <QAbstractItemModel>
#include <QModelindex>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QItemSelectionModel>
#include <QRegion>



LibSymbolView::LibSymbolView(QWidget *parent) :
    LibSymbolView(parent, nullptr, nullptr, nullptr/*, std::string()*/)
{

}

LibSymbolView::LibSymbolView(QWidget *parent, QAbstractItemModel *model) :
    LibSymbolView(parent, model, nullptr, nullptr/*, std::string()*/)
{

}

LibSymbolView::LibSymbolView(QWidget *parent, QAbstractItemModel *model, LibCore *pc, ILogger *pl )
   /*: LibClient(pc, pl, connpath) */
    //m_model(model)
{
    setModel(model);
}

void LibSymbolView::paintEvent(QPaintEvent *event) {
    QPainter qp(viewport());
    qp.drawRect(10,10,100,100);
}



// Public
QModelIndex LibSymbolView::indexAt(const QPoint &point) const  {
    return m_index;
}
void LibSymbolView::keyboardSearch(const QString &search) {

}
void LibSymbolView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)  {

}
void LibSymbolView::setSelectionModel(QItemSelectionModel *selectionModel) {

}
int LibSymbolView::sizeHintForColumn(int column) const {
    return 0;
}
int LibSymbolView::sizeHintForRow(int row) const {
    return 0;
}
QRect LibSymbolView::visualRect(const QModelIndex &index) const {
    QRect qr;
    return qr;
}

// Protected

int	LibSymbolView::horizontalOffset() const {
    return 0;
}
bool LibSymbolView::isIndexHidden(const QModelIndex &index) const {
    return false;
}
QModelIndex	LibSymbolView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers) {
    return m_index;
}
void LibSymbolView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags) {

}
int	LibSymbolView::verticalOffset() const {
    return 0;
}
QRegion	LibSymbolView::visualRegionForSelection(const QItemSelection &selection) const {
    QRegion r;
    return r;
}

