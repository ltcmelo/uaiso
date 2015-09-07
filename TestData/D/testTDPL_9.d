/+ Code from TDPL +/


auto
   a = 42,          // a has type int
   b = 42u,         // b has type uint
   c = 42UL,        // c has type ulong
   d = 4000000000,  // long; wouldn't fit in an int
   e = 4000000000u, // uint; it does fit in a uint
   f = 5000000000u; // ulong; wouldn't fit in a uint

auto c1 = '\xee';
static assert(is(typeof(c1) == char));
auto c2 = '\u1110';
static assert(is(typeof(c2) == wchar));
auto c3 = '\U00010010';
static assert(is(typeof(c3) == dchar));
auto c4 = '\&copy;';
static assert(is(typeof(c4) == dchar));
auto c5 = '\377';
static assert(is(typeof(c5) == char));

auto crlf = "\r\n";

// Failing!!!
//auto a = "This is a string with \"quotes\" in it, and also
//a newline, actually two" "\n";

auto somePrimes = [ 2u, 3, 5, 7, 11, 13 ];
auto someDoubles = [ 1.5, 3, 4.5 ];

static if (is(Widget[100][100] ManyWidgets)) {
   ManyWidgets lotsOfWidgets;

}

import std.stdio;

void main() {
   immutable(char)[3] a = "Hi!";
   immutable(char)[] b = a;
   writeln(a.length, " ", b.length); // Prints "3 3"
}
