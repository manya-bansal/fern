

A (semi-)reimplementation of Fern with clean(er) design decisions.


Stuff I am still skipping: 

- Some absolute paths in CMakeLists.txt that need to be passed in as CMake vars.
- Link to google benchmark properly (change benchmarking infra)
- Need to add a check for reorder
- There is a bug in the sliding window: need to change together all the deps to 
  figure out which interval var in local output is affected by which var in final
  output.
-  Place in the spot of the original host in the sliding window output, currently 
   returns the new host. Will not work if we try multiple.
-  Break and finalize do not interact well
-  Fork nodes needs to ignore inputs (finalize does this, but need to add an early exit to node merge)