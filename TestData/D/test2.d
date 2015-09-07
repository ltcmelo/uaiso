// Alias declarations
alias int a;
alias a = int;
alias abc = A;
alias a = A.B.C;
alias abc = Foo!(int);
alias abc = Foo!(int).t;
//alias a(T...) = T;
