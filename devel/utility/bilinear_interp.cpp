// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin bilinear_interp$$
$spell
   interpolant
   wg
   const
   CppAD
   interp
   dismod
$$

$section Bilinear Interpolation$$

$head Syntax$$
$icode%z% = bilinear_interp(
   %x%, %y%, %x_grid%, %y_grid%, %z_grid%, %i%, %j%
)%$$

$head x$$
This argument has prototype
$codei%
   double %x%
%$$
and is the $icode x$$ value at which we are
interpolating the bilinear function.

$head y$$
This argument has prototype
$codei%
   double %y%
%$$
and is the $icode y$$ value
at which we are interpolating the bilinear function.

$head x_grid$$
This argument has prototype
$codei%
   const CppAD::vector<double>& %x_grid%
%$$
and is the $icode x$$ values corresponding to the $icode z_grid$$.
We use the following notation:
$codei%
   %n_x% = %x_grid%.size()
%$$
Note that $icode n_x$$ must be greater than or equal one.

$head y_grid$$
This argument has prototype
$codei%
   const CppAD::vector<double>& %y_grid%
%$$
and is the $icode y$$ values corresponding to the $icode z_grid$$.
We use the following notation:
$codei%
   %n_y% = %y_grid%.size()
%$$
Note that $icode n_y$$ must be greater than or equal one.

$head z_grid$$
This argument has prototype
$codei%
   const CppAD::vector<double>& %z_grid%
%$$
and has size $icode%n_x% * %n_y%$$.
For $icode%i% = 0 , %...%, %n_x%-1%$$,
$icode%j% = 0 , %...%, %n_y%-1%$$,
$codei%
   %z_grid%[ %i% * %n_y% + %j%]
%$$
is the value of the bilinear interpolant $latex z(x,y)$$, at
$icode%x% = %x_grid%[%i%]%$$ and $icode%y% = %y_grid%[%j%]%$$.

$head i$$
This argument has prototype
$codei%
   size_t& %i%
%$$
and is between $code 0$$ and $icode%n_x%-1%$$ inclusive.
Its input value is a hint as to where to start searching for
$codei%
   %x_grid%[%i%] <= %x% < %x_grid%[%i%+1]
%$$
Its output value satisfies the condition above,
or is as close as possible under the limits
$codei%0 <= %i% < %n_x%$$.

$head j$$
This argument has prototype
$codei%
   size_t& %j%
%$$
and is between $code 0$$ and $icode%n_y%-1%$$ inclusive.
Its input value is a hint as to where to start searching for
$codei%
   %y_grid%[%j%] <= %y% < %y_grid%[%j%+1]
%$$
Its output value satisfies the condition above,
or is as close as possible under the limits
$codei%0 <= %j% < %n_y%$$.

$head z$$
The return value has prototype
$codei%
   double %z%
%$$
It is the value of the
$cref bilinear$$ interpolant at the specified
$icode x$$ and $icode y$$.

$children%example/devel/utility/bilinear_interp_xam.cpp
%$$
$head Example$$
The file $cref bilinear_interp_xam.cpp$$ contains an example and test
of using this routine.

$end
------------------------------------------------------------------------------
*/

# include <cppad/utility/nan.hpp>
# include <dismod_at/bilinear_interp.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

double bilinear_interp(
   double                         x          ,
   double                         y          ,
   const CppAD::vector<double>&   x_grid     ,
   const CppAD::vector<double>&   y_grid     ,
   const CppAD::vector<double>&   z_grid     ,
   size_t&                        i          ,
   size_t&                        j          )
{
   double nan  = CppAD::nan(0.0);
   size_t n_x  = x_grid.size();
   size_t n_y  = y_grid.size();
   assert( z_grid.size() == n_x * n_y );
   assert( i < n_x );
   assert( j < n_y );

   if( n_x == 1 && n_y == 1 )
      return z_grid[0];

   // find proper i
   if( x <= x_grid[0] )
      i = 0;
   else if( x_grid[n_x-1] <= x )
      i = n_x - 1;
   else
   {  assert( n_x > 1 );
      while( x < x_grid[i] )
      {  assert( 0 < i );
         i--;
      }
      assert( i+1 < n_x );
      while( x_grid[i+1] < x )
      {  i++;
         assert( i+1 < n_x );
      }
   }

   // find proper j
   if( y <= y_grid[0] )
      j = 0;
   else if( y_grid[n_y-1] <= y )
      j = n_y - 1;
   else
   {  assert( n_y > 1 );
      while( y < y_grid[j] )
      {  assert( 0 < j );
         j--;
      }
      assert( j+1 < n_y );
      while( y_grid[j+1] < y )
      {  j++;
         assert( j+1 < n_y );
      }
   }

   bool two_x  = (x_grid[0]  < x)  & (x  < x_grid[n_x-1]);
   bool two_y  = (y_grid[0]  < y)  & (y  < y_grid[n_y-1]);

   double x0  = x_grid[i];
   double y0  = y_grid[j];
   double z00 = z_grid[ i * n_y + j ];

   double x1(nan), z10(nan), dx(nan);
   if( two_x )
   {  x1  = x_grid[i+1];
      z10 = z_grid[ (i+1) * n_y + j];
      dx  = x1 - x0;
   }

   double y1(nan), z01(nan), dy(nan);
   if( two_y )
   {  y1  = y_grid[j+1];
      z01 = z_grid[ i * n_y + (j+1)];
      dy  = y1 - y0;
   }

   if( two_x & two_y )
   {  double z11 = z_grid[ (i+1) * n_y + (j+1) ];
      z00  = (x1 - x) * (y1 - y) * z00;
      z10  = (x - x0) * (y1 - y) * z10;
      z01  = (x1 - x) * (y - y0) * z01;
      z11  = (x - x0) * (y - y0) * z11;

      return ( z00 + z10 + z01 + z11 ) / (dx * dy);
   }
   if( two_x )
   {  z00 = (x1 - x) * z00;
      z10 = (x - x0) * z10;
      return (z00 + z10) / dx;
   }
   if( two_y )
   {  z00 = (y1 - y) * z00;
      z01 = (y - y0) * z01;
      return (z00 + z01) /dy;
   }
   return z00;
}

} // END_DISMOD_AT_NAMESPACE
