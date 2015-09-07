package pack

var (
    abcXyz  = []byte("abcXyz")
    fooBar, barFoo int = 1, 2
    here, there bool
)

type State interface {
    Write(b []byte) (ret int, err error)
    Width() (wid int, ok bool)
    Precision() (prec int, ok bool)
}

type Formatter interface {
    Format(f State, c rune)
}

type Stringer interface {
    String() string
}

func (b *buffer) Write(p []byte) (n int, err error) {
    //...
}

func (b *buffer) WriteString(s string) (n int, err error) {
    //...
}

var pool = sync.Pool{
    New: func() interface{} { return new(pp) },
}

func Printf(format string, a ...interface{}) (n int, err error) {
    //...
}

func Sprintf(format string, a ...interface{}) string {
    //...
}

func Errorf(format string, a ...interface{}) error {
    //...
}
