
#include <iostream>
 
using namespace std;
 
// 基类
class Shape 
{
   public:
      void setWidth(int w)
      {
         width = w;
      }
      void setHeight(int h)
      {
         height = h;
      }
      const char* getName()
      {
		  return pName;
	  }
	  int getID()
	  {
		  return identifyID;
	  }
   protected:
      int width;
      int height;
      
   private:
      const char *pName = "from outer world";
      int  identifyID = 20220208;
};
 
// 派生类
class Rectangle: public Shape
{
   public:
      int getArea()
      { 
         return (width * height); 
      }
      void getBasicName()
      {
		  name = getName();
	  }
	  void getBasicID()
	  {
		  id = getID();
	  }
	  
	  const char *name;
	  int id;
};
 
int main(void)
{
   Rectangle Rect;
 
   Rect.setWidth(5);
   Rect.setHeight(7);
   Rect.getBasicName();
   Rect.getBasicID();
 
   // 输出对象的面积
   cout << "Total area: " << Rect.getArea() << endl;
   
   cout << "identify name: " << Rect.name << endl;
   cout << "ID: " << Rect.id << endl;
 
   return 0;
}
