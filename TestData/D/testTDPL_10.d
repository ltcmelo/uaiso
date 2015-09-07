/+ Code from TDPL +/


bool find(int[] haystack, int needle) {
   foreach (v; haystack) {
      if (v == needle) return true;
   }
   return false;
}

int[] find(int[] haystack, int needle) {
   while (haystack.length > 0 && haystack[0] != needle) {
      haystack = haystack[1 .. $];
   }
   return haystack;
}

void fun(int x) { x += 42; }
void gun(int[] x) { x = [ 1, 2, 3 ]; }
void hun(int[] x) { x[0] = x[1]; }
unittest {
   int x = 10;
   fun(x);
   assert(x == 10);             // Unchanged
   int[] y = [ 10, 20, 30 ];
   gun(y);
   assert(y == [ 10, 20, 30 ]); // Unchanged
   hun(y);
   assert(y == [ 20, 20, 30 ]); // Changed!
}

int divrem(int a, int b, out int rem) {
   assert(b != 0);
   rem = a % b;
   return a / b;
}
unittest {
   int r;
   int d = divrem(5, 2, r);
   assert(d == 2 && r == 1);
}

static int zeros; // Practically the same as private int zeros;

void fun(int x) {
   static int calls;
   ++calls;
   if (!x) ++zeros;

}

T[] find(T)(T[] haystack, T needle) {
   while (haystack.length > 0 && haystack[0] != needle) {
      haystack = haystack[1 .. $];
   }
   return haystack;
}

T[] find(T, E)(T[] haystack, E needle) {
   while (haystack.length > 0 && haystack[0] != needle) {
      haystack = haystack[1 .. $];
   }
   return haystack;/*[\nopagebreak]*/
}

double average(double[] values...) {
   if (values.empty) {
      throw new Exception("Average of zero elements is undefined");
   }
   return reduce!((a, b) { return a + b; })(0.0, values)
      / values.length;
}

pure bool leapYear(uint y) {
   return (y % 4) == 0 && (y % 100 || (y % 400) == 0);
}

ulong fib(uint n) {
   return n < 2 ? n : fib(n - 1) + fib(n - 2);
}
