// Aggregate declarations
interface A;
interface A {}
interface B : A {}
interface C : B, O {}
class A {}
class B : A {}
class C : D, E {}
class A { class B {} }
final class A {}
abstract class A {}
const class A  {}
synchronized class A {}
class A { static class B {} }
struct S;
struct S { }
struct S { struct N {} }
struct AnonymousInside { struct { int a; } }
const struct S {}
immutable shared struct {}
struct S { static struct N {} }
union U;
union U {}
template Foo(T) {}
template Foo(T, U) {}
template Foo(T, U, V) {}
template Foo(int NT : 10) {}
template Foo(alias A) {}
template Foo(alias A, alias B) {}
template Foo(alias int A) {}
template Foo(T : A) {}
template Foo(T : T[U], U) {}
template Foo(T : U*, U : A) {}
template Foo(T: char) {}
template Foo(T : U!V, alias A) {}
template Bar(T : U!V, alias U, V...) {}
template Foo(T : U!(V), alias A) {}
template Foo(T...) {}
template Foo(T = int) {}
template Foo(T, U = T*) {}
interface I(T) {}
class A(T) {}
class List(T) : Base!T {}
struct A(T) {}
struct A(alias B) {}

// Constructors/destructors
class Foo
{
    this() {}
    this() shared;
    this() immutable;
    this() pure {}
    this(int[] x) immutable pure {}
    this(int x) {}
    static this();
    static this() {}
    private static this() {}
    @trusted shared static this() {}
    shared static this();
    shared static this() {}
    ~this();
    ~this() {}
    static ~this() {}
    shared static ~this() {}
}
