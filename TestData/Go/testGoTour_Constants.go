package main

import "fmt"

const Pi = 3.14

var (
    Abc = iota
    Mno
    Xyz
)

const (
    Abc = iota
    Mno
    Xyz
)

func main() {
        const World = "世界"
        fmt.Println("Hello", World)
        fmt.Println("Happy", Pi, "Day")

        const Truth = true
        fmt.Println("Go rules?", Truth)
}
