# Black-Scholes Example

One of the code examples provided is for the [Black-Scholes](https://en.wikipedia.org/wiki/Black%E2%80%93Scholes_model) option pricing model. This example demonstrates how to use Fern to generate a fused implementation of the Black-Scholes model.

The formula for this model can be found [here](https://gosmej1977.blogspot.com/2013/02/black-and-scholes-formula.html). Given the stock price, strike price, and volatility of a stock, as well as the interest rate and time to maturity, we can calculate the option price using the formula. We want to calculate this for many stocks, so we take in the stock price, strike price, and volatility as arrays, with a constant interest rate and time to maturity.

## Implementation

This example is implemented in `test/test-pipeline.cpp`, in the `BlackScholes` test.

### Variables

First, we define the arrays that will store our data. For example, here's the array that stores the stock price.

```cpp
examples::Array<float> stock_price("stock_price");
```

This makes use of the `Array` class in `include/examples/pointwise-array.h`.

We also need to know how the length of the arrays we are processing. This is done by declaring a variable with the second argument as `true` (meaning this variable should be an argument in the final generated function).

```cpp
Variable arg_len("arg_len", true);
```

### Pipeline

Now, let's take a look at our pipeline. The pipeline is initialized with a vector of functions -- these functions describe how we transform our data and Fern will fuse them together.

The first function we pass in is:

```cpp
div_mock(DataStructureArg(&stock_price, "data"), DataStructureArg(&strike_price, "data"), getNode(arg_len), DataStructureArg(&s_over_k, "data"))
```

Here, we're calculating `stock_price / strike_price` element-wise over the `stock_price` and `strike_price` arrays.

The `div_mock` function is an instance of our `examples::div_mock` class, which can also be found in `include/examples/pointwise-array.h`.

The important methods of this class are

- `getName()`: This is the name of our external function (how it will be called in the generated code). If we are using an external library function, we should put its name here.
- `getDataRelationship()`: This describes how fern will loop over the data. We define `loop` as a `fern::Interval` that loops over every element in the array, and annotate the two input arrays as the data consumed and the output array as the data produced. Fern will use this information to determine how to fuse the functions together.o
- `getArguments()`: This returns a vector of the arguments to the function. We specify that the function takes in the two arrays we are dividing, the length, as well as an array to store the output in.

Our entire pipeline is made up of all the functions we need to call to calculate the Black-Scholes formula. Fern will use the data relationship annotations we provide to figure out which arrays are true inputs in the generated function and which are true outputs. The intermediate arrays will not be exposed as parameters in the generated function.

### Code Generation

Finally, we run these lines to construct and finalize the pipeline, and print the generated code to a file. The `black_scholes.ir` file will contain an intermediate representation of our fused function, whereas the `black_scholes_impl.cpp` file will contain the C++ code of the fused function.

```cpp
pipeline.constructPipeline();
pipeline = pipeline.finalize();

util::printToFile(pipeline,
                  std::string(SOURCE_DIR) + "/code_sample" + "/black_scholes.ir");
codegen::CodeGenerator code(pipeline);
util::printToFile(code, std::string(SOURCE_DIR) + "/code_sample/code" +
                          "/black_scholes_impl.cpp");
```

## Generated Code

The generated code has the following signature:

```cpp
void my_fused_impl(const Array<float> strike_price, const Array<float> stock_price, const Array<float> volatility, Array<float> out, int64_t len194);
```

We can see that it takes in the strike price, stock price, and volatility as arrays, and the length of the arrays. The `out` array is the output array that will store the calculated option prices.

All of the intermediate arrays we defined are not exposed as parameters.

### Running

The file `code_sample/code/black_scholes.cpp` contains an example of calling `my_fused_impl` to run the Black-Scholes example. It verifies that the results are correct by comparing to a naive, unfused implementation of the Black-Scholes formula.
