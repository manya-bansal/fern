/*
<Fern Annotation>
x_tile : Symbolic
y_tile : Symbolic
stride_arg : Symbolic
for x in [output.W_start, output.W_start + output.W_len, x_tile]{
    for y in [output.H_start, output.H_start + output.H_len, y_tile]{
        produce{
            output {
                x: x,
                y: y,
                width: x_tile,
                length: y_tile
            }
        }
        when consume{
            input {
                x: x * stride_arg,
                y: y * stride_arg,
                width: x_tile + filter.H - 1,
                length: x_tile + filter.W - 1,
            }
        }
    }
}
</Fern Annotation>
*/
void convolution_mkl_test(Weights input, Weights filter, Weights bias,
                          int stride_arg, Weights output);