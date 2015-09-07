// Module and import declarations
module test5;
import foo;
import foo, bar;
import list = util.container.linear.list;
import io = std.stdio : foo = writeln;
import io = std.stdio : writeln, write;
import io = std.stdio : writeln, foo = write;
import io = std.stdio : say = writeln, foo = write;
import std.stdio : say = writeln, cfile = File;
import widget : fun, gun;
static import acme.goodies.io;
public import acme.goodies.io;
int i; // Make the environment non-empty (certain tests rely on it)
