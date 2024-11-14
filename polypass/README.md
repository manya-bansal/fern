## PolyPass

Generating Fern annotations automatically for simple ```nd```-array code that the MLIR Affine dialect can actually analyze.
Want to build a MINIMAL MLIR pass skeleton.


 /Users/manya227/llvm-project/build/bin/clang -S -c -emit-llvm test.c

 ```
 /Users/manya227/llvm-project/build/bin/clang test.c -c -O0 -Xclang -disable-O0-optnone -emit-llvm -S -o - | /Users/manya227/llvm-project/build/bin/opt -polly-canonicalize -S


  /Users/manya227/llvm-project/build/bin/clang test.c -c -O0 -Xclang -disable-O0-optnone -emit-llvm -S -o - | /Users/manya227/llvm-project/build/bin/opt -polly-canonicalize -polly-export-jscop -S


 ```

```
/Users/manya227/llvm-project/build/bin/clang test.c -c -O0 -Xclang -disable-O0-optnone -emit-llvm -S -o - | /Users/manya227/llvm-project/build/bin/opt -polly-canonicalize -polly-print-dependences -disable-output -S
```

```
/Users/manya227/llvm-project/build/bin/clang test.c -c -O3 -ffast-math -Xclang -disable-O0-optnone -emit-llvm -S -o - | /Users/manya227/llvm-project/build/bin/opt -polly-canonicalize -polly-print-dependences -polly-process-unprofitable -polly-use-llvm-names -basic-aa -disable-output -polly-scops -S
```

Use 
```
-polly-use-llvm-names
```

To get names back!