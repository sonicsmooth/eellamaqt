#ifndef LIBSYMBOLVIEW_H
#define LIBSYWBOLVIEW_H

#include "libclient.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QPaintEvent>
#include <QRect>
#include <QItemSelectionModel>
#include <QRegion>

#include <string>

class LibSymbolView : public LibClient, public QAbstractItemView
{
private:
    QModelIndex m_index;
public:
    LibSymbolView(QWidget * = nullptr);
    LibSymbolView(QWidget * = nullptr, QAbstractItemModel * = nullptr);
    LibSymbolView(QWidget * = nullptr, QAbstractItemModel * = nullptr, LibCore * = nullptr, ILogger * = nullptr, std::string = std::string());
    void paintEvent(QPaintEvent *event) override;

    // Reimplemented public functions
    QModelIndex	indexAt(const QPoint &) const  override;
    void keyboardSearch(const QString &) override;
    void scrollTo(const QModelIndex &, QAbstractItemView::ScrollHint hint = EnsureVisible)  override;
    void setModel(QAbstractItemModel *) override;
    void setSelectionModel(QItemSelectionModel *) override;
    int	sizeHintForColumn(int) const override;
    int	sizeHintForRow(int) const override;
    QRect visualRect(const QModelIndex &) const  override;

    // Reimplemented protected functions
    virtual int	horizontalOffset() const override;
    virtual bool isIndexHidden(const QModelIndex &) const override;
    virtual QModelIndex	moveCursor(QAbstractItemView::CursorAction, Qt::KeyboardModifiers) override;
    virtual void setSelection(const QRect &, QItemSelectionModel::SelectionFlags) override;
    virtual int	verticalOffset() const override;
    virtual QRegion	visualRegionForSelection(const QItemSelection &) const override;
};


#endif // LIBSYMBOLVIEW_H
