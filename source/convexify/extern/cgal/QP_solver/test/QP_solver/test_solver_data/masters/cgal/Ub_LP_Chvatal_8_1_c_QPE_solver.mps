* Number-type: integer
* Description: Example 8.1c from Chvatal
* Generated-by: Generated by Ub_LP_Chvatal_8_1_c_QPE_solver.mw
NAME Ub_LP_Chvatal_8_1_c_QPE_solver
ROWS
 N obj
 L c1
 L c2
 E c3
COLUMNS
  x1  c1  3
  x1  c2  -5
  x1  c3  1
  x1  obj  -5
  x2  c1  1
  x2  c2  4
  x2  c3  1
  x2  obj  -2
  x3  c1  -4
  x3  c2  2
  x3  c3  2
  x3  obj  3
  x4  c1  2
  x4  c2  -3
  x4  c3  1
  x4  obj  -3
  x5  c1  5
  x5  c2  2
  x5  c3  1
  x5  obj  -6
  x6  c1  1
  x6  c2  3
  x6  c3  2
  x6  obj  -1
RHS
  rhs  c1  3
  rhs  c2  25
  rhs  c3  4
BOUNDS
  LO  BND  x2  2
  UP  BND  x2  10
  MI  BND  x3
  UP  BND  x3  0
  LO  BND  x4  -3
  UP  BND  x4  3
  MI  BND  x5
  MI  BND  x6
ENDATA
