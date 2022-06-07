//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// The sum program (Chapel version 2, domain-mapped).
//
//
use BlockDist;
config const N = 1000;	/* problem domain size */
const D = {0..N-1};
const BlockD = D dmapped Block(boundingBox=D);
var sum: int;

proc compute(i: int): int { return i*i; }

sum = + reduce [i in BlockD] (compute(i));
writeln("The sum is ", sum, " (should be 332833500)");
