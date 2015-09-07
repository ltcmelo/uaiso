/+ Code from TDPL +/

// Context begin
import std.stdio;
int feet, inches;
double inchesPerFoot = 0, cmPerInch;
// Context end
unittest {
foreach (feet; 5 .. 7)
  foreach (inches; 0 .. inchesPerFoot)
    writefln("%s'%s''\t%s", feet, inches,
      (feet * inchesPerFoot + inches) * cmPerInch);
}


void main(){}
