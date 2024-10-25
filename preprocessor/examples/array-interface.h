/*
<Fern Annotation>
len : Symbolic
for i in [A.idx, A.idx + A.size, len]{
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
void vadd(Array<float> B, Array<float> C, Array<float> A);
