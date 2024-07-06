

A (semi-) reimplementation of Fern with cleaner design decisions.


Stuff I am still skipping: 

- Some absolute paths in CMakeLists.txt that need to be passed in as CMake vars.
- Link to google benchmark properly (change benchmarking infra)
- Need to add a check for reorder
- There is a bug in the sliding window: need to change together all the deps to 
  figure out which interval var in local output is affected by which var in final
  output.