# include <cstdlib>
# include <iostream>

/* Nikola: 
`val_test01.cpp` computes the first 10 Fibonacci numbers. It contains two memory errors,
1. the loop in `f` goes too far, accessing and writing to un-allocated memory.
2. `malloc`'d memory is freed using `delete`. This has undefined behavior, 
    and is fixed by instead using `free`.
These bugs were found and confirmed to be fixed using `valgrind --tool=memcheck val_test01`.
*/

using namespace std;

int main ( );
void f ( int n );

//****************************************************************************80

int main ( )

//****************************************************************************80
//
//  Purpose:
//
//    MAIN is the main program for TEST01.
//
//  Discussion:
//
//    TEST01 calls F, which has a memory "leak".  This memory leak can be
//    detected by VALGRID.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    18 May 2011
//
{
  int n = 10;

  cout << "\n";
  cout << "TEST01\n";
  cout << "  C++ version.\n";
  cout << "  A sample code for analysis by VALGRIND.\n";

  f ( n );
//
//  Terminate.
//
  cout << "\n";
  cout << "TEST01\n";
  cout << "  Normal end of execution.\n";

  return 0;
}
//****************************************************************************80

void f ( int n )

//****************************************************************************80
//
//  Purpose:
//
//    F computes N+1 entries of the Fibonacci sequence.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    18 May 2011
//
{
  int i;
  int *x;

  x = ( int * ) malloc ( n * sizeof ( int ) );

  x[0] = 1;
  cout << "  " << 0 << "  " << x[0] << "\n";

  x[1] = 1;
  cout << "  " << 1 << "  " << x[1] << "\n";

  for ( i = 2; i < n; i++ )
  {
    x[i] = x[i-1] + x[i-2];
    cout << "  " << i << "  " << x[i] << "\n";
  }

  free(x);

  return;
}
