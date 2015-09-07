package main

import (
        "fmt"
        "math/cmplx"
)

var (
        ToBe , NoToBe  bool = false, true
        TestMe = false
        MaxInt uint64     = 1<<64 - 1
        // MaxInt uint64     = 1<<64 - 1
         MaxInt2 uint64     = 1<<64 - 1 // Comment after
         MaxInt3 uint64     = 1<<64 - 1; // Comment after
        z      complex128 = cmplx.Sqrt(-5 + 12i)
)

func main() {
        const f = "%T(%v)\n"
        fmt.Printf(f, ToBe, ToBe)
        fmt.Printf(f, MaxInt, MaxInt)
        fmt.Printf(f, z, z)
}
