#ifndef LIBEDCORE_H
#define LIBEDCORE_H

#include <string>
#include <vector>
#include <initializer_list>

#include "ilogger.h"

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

class LibEdCore
{
private:
    ILogger* m_pLogger;
public:
    LibEdCore();
    ~LibEdCore();
    void setLogger(ILogger*);
    void New(std::string);
    void Open(std::string);
    void Save(std::string);
    void Close(std::string);
    void Quit();
    // Symbols are created/deleted
    // Shapes are created/deleted then inserted/removed with position/rotation, and can be reused
    // Primitives are created/deleted with parent, and cannot be reused
    void Create(Symbol);
    void Create(Shape);
    void Delete(Symbol);
    void Delete(Shape);
    void Rename(Symbol, std::string);
    void Rename(Shape, std::string);
    void InsertShape(Symbol, Shape, double x, double y);
    void RemoveShape(Symbol, Shape);
    void TranslateShape(Symbol, Shape, double x, double y);
    void RotateShape(Symbol, Shape, double a);
    void InsertPrimitive(Symbol, Primitive);
    void InsertPrimitive(Shape, Primitive);
    void RemovePrimitive(Symbol, Primitive);
    void RemovePrimitive(Shape, Primitive);
};

#endif // LIBEDCORE_H
