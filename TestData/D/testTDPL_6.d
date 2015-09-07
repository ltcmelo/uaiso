/+ Code from TDPL +/


import std.array;

bool binarySearch(T)(T[] input, T value) {
   while (!input.empty) {
      auto i = input.length / 2;
      auto mid = input[i];
      if (mid > value) input = input[0 .. i];
      else if (mid < value) input = input[i + 1 .. $];
      else return true;
   }
   return false;
}
