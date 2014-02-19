// dynamic allocation and polymorphism
#include <iostream>
using namespace std;

class Polygon {
  protected:
    int width, height;
  public:
  Polygon (int a, int b, string name) : width(a), height(b), name(name) {}
    string name;
    virtual int area (void) =0;
  //virtual int area (void);
    void printarea()
  { cout << "area of a "<<name<<"angle is "<<this->area() << '\n'; }
};

class Rectangle: public Polygon {
  public:
    
  Rectangle(int a,int b) : Polygon(a,b,"rect") {}
    int area()
      { return width*height; }
};

class Triangle: public Polygon {
  public:
  Triangle(int a,int b) : Polygon(a,b,"tri"){}
    int area()
      { return width*height/2; }
};

int main () {
  Polygon * ppoly1 = new Rectangle (4,5);
  Polygon * ppoly2 = new Triangle (4,5);
  ppoly1->printarea();
  ppoly2->printarea();
  delete ppoly1;
  delete ppoly2;
  return 0;
}
