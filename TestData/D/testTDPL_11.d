/+ Code from TDPL +/

class Widget {
   // A constant
   enum fudgeFactor = 0.2;
   // A shared immutable value
   static immutable defaultName = "A Widget";
   // Some state allocated for each Widget object
   string name = defaultName;
   uint width, height;
   // A static method
   static double howFudgy() {
      return fudgeFactor;
   }
   // A method
   void changeName(string another) {
      name = another;
   }
   // A non-overridable method
   final void quadrupleSize() {
      width *= 2;
      height *= 2;
   }
}

class Test {
   double a = 0.4;
   int b;
   this(int b) {
      this.b = b;
   }
}

class Test {
   double a = 0.4;
   int b;
   this(int b) {
      this.b = b;
   }
   this() {} // Default constructor,
             //    all fields implicitly initialized
}

class NoGo {
   void fun() {
      // Let's just rebind this to a different object
      this = new NoGo; // /*[\codeError]*/ Cannot modify `this'!
   }
}
class Buffer {
   private void* data;
   // Constructor
   this() {
      data = malloc(1024);
   }
   // Destructor
   ~this() {
      free(data);
   }

}

class A {
   static A a1;
   static this() {
      a1 = new A;
   }

   static this() {
      a2 = new A;
   }
   static A a2;
}

class A {
   static A a1;
   static ~this() {
      clear(a1);
   }

   static A a2;
   static ~this() {
      clear(a2);
   }
}

class Contact { }
class Friend : Contact { }

class Contact { string bgColor() { return ""; } }
class Friend : Contact {
   override string bgColor() { return "LightGreen"; }
}

class Clickable {
   void onClick() { }
}
class Button : Clickable {
   void drawClicked() { }
   override void onClick() {
      drawClicked();    // Introduce graphical effect
      super.onClick();  // Dispatch click to listeners
   }
}

class C {
   // x is accessible only inside this file
   private int x;
   // This file plus all classes inheriting C directly or
   //    indirectly may call setX()
   protected void setX(int x) { this.x = x; }
   // Everybody may call getX()
   public int getX() { return x; }
}

interface Transmogrifier {
   // Client interface
   final void thereAndBack() {
      transmogrify();
      untransmogrify();
   }
   // Implementation interface
private:
   void transmogrify() { }
   void untransmogrify() { }
}

class QuiteAbstract {
   abstract {
      // Everything in this scope is abstract
      void fun();
      int gun();
      double hun(string);
   }
}
