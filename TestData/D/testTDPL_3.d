/+ Code from TDPL +/
import std.stdio;

void fun(ref uint x, double y) {
   x = 42;
   y = 3.14;
}

void main() {
   uint a = 1;
   double b = 2;
   fun(a, b);
   writeln(a, " ", b);
}
