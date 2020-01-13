#ifndef LIBCORE_H
#define LIBCORE_H

#include "loggable.h"
#include "idbif.h"
#include "iuimanager.h"

#include <string>
#include <vector>
#include <list>
#include <optional>
#include <initializer_list>


class Shape {
private:
    std::string m_name;
public:
    Shape(std::string name) : m_name(name) {}
    Shape & operator=(std::string &&name) {m_name = name; return *this;}
    std::string name() { return m_name; }
};
class Symbol {
private:
    std::string m_name;
public:
    Symbol(std::string name) : m_name(name) {}
    Symbol operator=(std::string &&name) {m_name = name; return *this;}
    std::string name() { return m_name; }
};

class Primitive {
private:
    std::string m_name;
public:
    virtual ~Primitive() {}
    void setName(std::string name) {m_name = name;}
    std::string name() {return m_name;}
};
class Rectangle : Primitive {
public:
    double x,y,w,h;
    Rectangle(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {}
};
class Circle : Primitive {
public:
    double x, y, r;
    Circle(double x, double y, double r) : x(x), y(y), r(r) {}
};
class Line : Primitive {
public:
    double x1, y1, x2, y2;
    Line(double x1, double y1, double x2, double y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
};
class Text : Primitive {
public:
    double x, y;
    std::string txt;
    Text(double x, double y, std::string txt) : x(x), y(y), txt(txt) {}
};

class LibCore : public Loggable
{
private:
    IDbIf* m_pDbIf;
    std::list<std::string> m_activeDb; // maybe move this to dbif??
    IUIManager *m_pUIManager;

public:

    enum class DupOptions {CLOSE_OLD, OPEN_NEW, QUIETLY, RENAME};

    LibCore();
    ~LibCore();
    void setDbIf(IDbIf*);
    IDbIf *DbIf() const;
    void setUIManager(IUIManager *);
    IUIManager *UIManager() const;
    std::optional<std::string> activeDb() const; // returns top active Db or null_opt
    bool activeDb(std::string) const; // returns string in activeDb list or null_opt
    void pushActiveDb(std::string);
    void popActiveDb(std::string);
    void newLib(std::string);
    void openLib(std::string);
    void saveLib(std::string, std::string, DupOptions);
    void closeActiveLib();
    void closeLib(std::string);
    void deleteActiveLib();
    void deleteLib(std::string);

    // Symbols are created/deleted
    // Shapes are created/deleted then inserted/removed with position/rotation, and can be reused
    // Primitives are created/deleted with parent, and cannot be reused

    void newShape();
    void newSymbol();
    void deleteShape(std::string name);
    void deleteSymbol(std::string name);
    void renameShape(std::string name);
    void renameSymbol(std::string name);

    // void insertShape(std::string dbConn, std::string symbolName, Shape, double x, double y);
    // void removeShape(std::string dbConn, std::string symbolName, std::string shapeName);
    // void translateShape(Symbol, Shape, double x, double y);
    // void rotateShape(Symbol, Shape, double a);
    // void insertPrimitive(Symbol, Primitive);
    // void insertPrimitive(Shape, Primitive);
    // void removePrimitive(Symbol, Primitive);
    // void removePrimitive(Shape, Primitive);
};

#endif // LibCore_H
