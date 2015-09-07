/+ Code from TDPL +/


import std.stdio;

struct MyStruct {
   int data;
}
class MyClass {
   int data;
}

void main() {
   // Play with a MyStruct object
   MyStruct s1;
   MyStruct s2 = s1;
   ++s2.data;
   writeln(s1.data); // Prints 0
   // Play with a MyClass object
   MyClass c1 = new MyClass;
   MyClass c2 = c1;
   ++c2.data;
   writeln(c1.data); // Prints 1
}
