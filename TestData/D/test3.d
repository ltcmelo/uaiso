// Function declarations
int foo() {}
int foo();
int[] foo();
double foo(void);
void foo(int);
void foo(int i);
void foo(int, double);
void foo(int i, double j);
int foo(int i, int y, ...); // Regular variadic function
int foo(...);
void foo(int[] a);
void foo(int[10] a);
void foo(int[] a...); // Typesafe variadic function
void foo(int[] a ...);
void foo(int[10] a ...);
void foo(int a, Type t);
void foo(int a, Type t ...);
void foo(int a, Type[] t ...);
A foo();
A[] foo();
static void foo() {}
void foo(T)() {}
void foo(T, U)() {}
void foo(T, U)(int) {}
void foo(T, U)(T, U) {}
void foo(T, U)(T t, U u) {}
void foo(T = int, U)() {}
void foo(T = int, U = A)() {}
void foo(T, U = T)() {}
void foo(T : T*, U = A)() {}
auto foo(T)(T t) {}
auto foo(int a);
auto foo(int a, int b = 10) {}
pure int foo(int i,
             char* p,
             const char* q,
             immutable int* s);
void foo() nothrow {}
long delegate(int) foo() {}
ref int foo();
ref auto foo(int x);
auto ref foo(int x);
auto ref foo();
auto ref foo(ref int x);
auto ref foo(out int x);
auto ref foo(in int x);
auto ref foo(lazy int x);
auto ref foo() { }
int[] foo(int[] a, int x, int y);
const(int)[] foo(const(int)[] a, int x, int y);
immutable(int)[] foo(immutable(int)[] a, int x, int y);
inout(int)[] foo(inout(int)[] a, int x, int y);
@property int foo();
@property ref int foo();
int def() {}
final int foo() {}
final private int bar() {}
private int abc() {}
override int def() {}
private static string a() {}
void a(void* p = null) @trusted pure nothrow;
void a(void* p = null) pure @safe nothrow;
private: void f() {}
immutable : void foo();
//@safe pure nothrow { void f(); void bar(); }
immutable { void f(); void bar(); }
const int f() { }
immutable const int f() {}
//immutable immutable(int) f(); // Equivalent to declaration below - TODO: Parser not handling properly
immutable(int) f() immutable; // Equivalent to declaration above
long foo(long x) in {} out (result) {} body {}
