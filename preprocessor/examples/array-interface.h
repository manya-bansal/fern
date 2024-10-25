/*
<Fern Annotation>
len : Symbolic
for i in [a.idx, a.idx + a.size, len]{
    produce{
        A {
            idx: i,
            length: len
        }
    }
    when consume{
        B {
            idx: i,
            length: len
        },
        C {
            idx: i,
            length: len
        }
    }
}
</Fern Annotation>
*/
template <typename T> void vadd(Array<T> a, Array<T> b, Array<T> out);
