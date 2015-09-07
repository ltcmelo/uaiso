/+ Code from TDPL +/


int printf(const char * format,);

alias immutable(int) StableInt;
StableInt forever = 42;

void fun(int x) {
   immutable(int) xEntry = x;

}

immutable pi = 3.14, val = 42;

struct DataSample {
   int id;
   double[] payload;
}
alias immutable(int[]) T1;
alias immutable(int)[] T2;

class A {
   int a;
   int[] b;
   this() immutable {
      a = 5;
      b = [ 1, 2, 3 ];
      // Calling fun() wouldn't be allowed
   }
   void fun() immutable {

   }
}

import std.conv;

struct S {
   private int[] a;
   // Conversion from immutable to mutable
   this(immutable(S) source) {
      // Duplicate the array into a non-immutable array
      a = to!(int[])(source.a);
   }
   // Conversion from mutable to immutable
   this(S source) immutable {
      // Duplicate the array into an immutable array

      //TODO: Failing
      //a = to!(immutable(int[]))(source.a);
   }

}

void print(const(int[]) data) { }
unittest {
   immutable(int[]) myData = [ 10, 20 ];
   print(myData); // Fine
   int[] myMutableData = [ 32, 42 ];
   print(myMutableData); // Fine
}

char* strchr(const char* input, int c);

inout(char)* strchr(inout(char)* input, int c);
