// Statements
void f()
{
    foreach (a; 0 .. b) {}
    foreach (a; 0 .. b.c) {}
    foreach (a; 0 .. char.c) {}
    foreach (a; 0 .. (char).c) {}
    foreach (double a; 0 .. (char).c) {}
}
