// Enum declarations
enum A = 3;
enum long A = 3;
enum { A, B, C }
enum { A, B, C,  }
enum X { A, B, C }
enum X { A, B, C, }
enum X { A = 3, B, C }
enum X { A = 3, B, C, }
enum C
{
    A = B,
    B = D,
    C = 3,
    D
}
enum E : C
{
    E1 = C.A,
    E2
}
enum { A, B = 5 + 7, C, D = 8 + C, E }
enum : long { A = 3, B }
enum : string
{
    A = "hello",
    B = "betty"
}
enum
{
    A = "string",
    B,
    int C = 3,
    D
}
enum string constant(TL...) = TL.stringof;

