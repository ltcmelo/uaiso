package main

var i int
var j int = 9
var k *int
var m chan int
var a A
var b B = B{1, 2}
var c C = pack.C{1, 2}

func f() {}
func f() { return 1 }
func f() int { return 1 }
func f(x int) int { return x }
func f(x int, y int) { return x + y }
func f(x, y int) int { return x + y }
func f(x, y int, m, n string) int { return x + y }
func f(x, y) int { return 1 } // Unnamed params of type x and y.
func f(int, int, int) int { return 1 }
func f(x, y int) (int, int) { return a, b }
func f(x, y int) (a int, b int) { return a, b }
func f(x, y int) (a A, b B) { return a, b }

type Vertex struct {
        a int
        b int
}
