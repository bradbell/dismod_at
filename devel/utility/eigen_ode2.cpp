// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin eigen_ode2}
{xrst_spell
   cc
   eigen
   eigenvalue
   eigenvector
   tf
   yf
   yi
}

Eigen Vector Solution of ODE with Two Components
################################################

Syntax
******

   *yf* = ``eigen_ode2`` ( *case_number* , *b* , *yi* , *tf* )

Purpose
*******
Given :math:`y( 0 ) \in \B{R}^2`,
:math:`B \in \B{R}^{2 \times 2}`,
and :math:`t_f \in \B{R}_+`,
this routine uses the eigen vectors of :math:`B` to solve for
:math:`y( t_f )` where

.. math::

   y' (t) = B y(t)

Furthermore, the operation sequence only depends on the value of
*case_number* and not on the value of the particular *b*
for which this function is recorded.

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

b
*
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *b*

and size four.
It specifies the matrix :math:`B`
in row-major order; i.e.,

.. math::

   B
   =
   \left( \begin{array}{cc}
      B_{0,0}  & B_{0,1} \\
      B_{1,0}  & B_{1,1}
   \end{array} \right)
   =
   \left( \begin{array}{cc}
      b_0  & b_1 \\
      b_2  & b_3
   \end{array} \right)

Assumption
**********
We restrict our attention to the case where the off-diagonal elements
of :math:`B` have the same sign; i.e.

.. math::

   B_{0,1} B_{1,0} = b_1 b_2 \geq 0

yi
**
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *yi*

and size two.
It specifies the vector :math:`y( 0 )`, to be specific,

|tab| :math:`y_0 ( 0 ) =` *yi* [0]

|tab| :math:`y_1 ( 0 ) =` *yi* [1] .

tf
**
This argument has prototype

   ``const`` *Float* & *tf*

It specifies the final time; i.e. :math:`t_f`.

yf
**
The return value has prototype

   ``CppAD::vector<`` *Float* >& *yf*

and size two and contains the solution of the ODE; i.e.,

   ``yf`` [0]

:math:`= y_0 ( t_f )`

   ``yf`` [1]

:math:`= y_1 ( t_f )`.

case_number
***********
This argument has prototype

   ``size_t`` *case_number*

One
===
The case where :math:`b_1 = 0` and :math:`b_2 = 0` we denote by

   *case_number*  == 1

In this case we return the solution

.. math::
   :nowrap:

   \begin{eqnarray}
      y_0 ( t_f ) & = & y_0 (0) \exp( b_0 t )
      \\
      y_1 ( t_f ) & = & y_1 (0) \exp( b_3 t )
   \end{eqnarray}

Two
===
The case where :math:`b_1 \neq 0` and :math:`b_2 = 0`.
We denote by

   *case_number*  == 2

In this case, we switch the order of the rows and columns in
:math:`B` and *yi* ,
compute the solution using
:ref:`eigen_ode2@Method@Case Three`
and then switch the order of the result.

Three
=====
The case where :math:`b_1 = 0`, :math:`b_2 \neq 0`.
We denote this case by

   *case_number*  == 3

In this case, we compute the solution use the method for
:ref:`eigen_ode2@Method@Case Three` below.

Four
====
The case where :math:`b_1 \neq 0` and :math:`b_2 \neq 0`.

   *case_number*  == 4

In this case, we compute the solution use the method for
:ref:`eigen_ode2@Method@Case Four` below.

Method
******
The solution for case one is presented above.
The solution for case two is to convert it to case three.

Case Three
==========
For this case :math:`b_1 = 0` and :math:`b_2 \neq 0`.
It follows that

.. math::
   :nowrap:

   \begin{eqnarray}
   y_0 ( t )   & = & y_0 ( 0 ) \exp ( b_0 t )
   \\
   y_1 ' ( t ) & = & b_3 y_1 (t) + b_2 y_0 ( t )
   \\
   y_1 (t)     & = & y_1 ( 0 ) \exp ( b_3 t ) +
      \int_0^t \exp [ b_3 ( t - s ) ] b_2 y_0 (s) \B{d} s
   \\
   y_1 (t)     & = & y_1 ( 0 ) \exp ( b_3 t ) +
      b_2 y_0 ( 0 ) \int_0^t \exp [ b_3 ( t - s ) + b_0 s  ] \B{d} s
   \\
   y_1 (t)     & = & y_1 ( 0 ) \exp ( b_3 t ) +
      b_2 y_0 ( 0 ) \exp ( b_3 t ) \int_0^t \exp [ ( b_0 - b_3 ) s ] \B{d} s
   \end{eqnarray}

Case Four
=========
In this case

.. math::

   (b_0 - b_3)^2 + b_1 b_2 > 0

A value :math:`\lambda`
is an eigen value of :math:`B` if and only if:

.. math::
   :nowrap:

   \begin{eqnarray}
   0 & = & ( b_0 - \lambda ) ( b_3 - \lambda )  - b_1 b_2
   \\
   0 & = & \lambda^2 - (b_0 + b_3) \lambda + b_0 b_3 - b_1 b_2
   \\
   \lambda & = &  \frac{
      (b_0 + b_3) \pm \sqrt{ (b_0 + b_3)^2 - 4 b_0 b_3 + 4 b_1 b_2 }
   }{2}
   \\
   \lambda & = &  \frac{
      (b_0 + b_3) \pm \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
   }{2}
   \end{eqnarray}

We can express an arbitrary left eigenvector of :math:`B`
as :math:`(1, u)` where :math:`u \in \B{R}`,

.. math::
   :nowrap:

   \begin{eqnarray}
      \lambda & = & b_0  + b_2 u
      \\
      \lambda u & = & b_1  + b_3 u
   \end{eqnarray}

and :math:`\lambda` is an eigenvalue of :math:`B`.
Using the first equation to solve for :math:`u`, we have

.. math::

   \lambda \; [ 1 \; , \; ( \lambda - b_0 ) / b_2  ]
   =
   [ 1 \; , \; ( \lambda - b_0 ) / b_2  ] \; B

where

.. math::

   \lambda =  \frac{
      (b_0 + b_3) \pm \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
   }{2}

Define :math:`\lambda_+` and :math:`\lambda_-` as corresponding
to the plus and minus the square root above and note
:math:`\lambda_+ \neq \lambda_-` because :math:`b_1 b_2 > 0`.
We also define

.. math::
   :nowrap:

   \begin{eqnarray}
      u_\pm & = & ( \lambda_\pm - b_0 ) / b_2
      \\
      z_\pm (t) & = & y_0 (t) + u_\pm y_1 (t)
   \end{eqnarray}

It follows that

.. math::
   :nowrap:

   \begin{eqnarray}
   z_\pm (t) & = &  (1, u_\pm ) y(t)
   \\
   z_\pm ' (t)
   & = &  (1, u_\pm ) y'(t) =  (1, u_\pm ) B y (t) = \lambda_\pm z(t)
   \\
   z_\pm (t) & = & z_\pm (0) \exp( \lambda_\pm t )
   \end{eqnarray}

Which enables us to compute :math:`z_\pm (t)`.
Furthermore

.. math::
   :nowrap:

   \begin{eqnarray}
   y_1 (t) & = & [ z_+ (t) - z_- (t) ] / ( u_+ - u_- )
   \\
   & = & [ z_+ (t) - z_- (t) ] b_2 / ( \lambda_+ - \lambda_- )
   \\
   & = & [ z_+ (t) - z_- (t) ] b_2 / \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
   \\
   y_0(t) & = & z_+ (t) - u_+ y_1 (t)
   \end{eqnarray}

{xrst_toc_hidden
   example/devel/utility/eigen_ode2_xam.cpp
}
Example
*******
The file :ref:`eigen_ode2_xam.cpp-name` contains
and example and test of ``eigen_ode2`` .
It returns true for success and false for failure.

{xrst_end eigen_ode2}
---------------------------------------------------------------------------
*/
# include <cppad/cppad.hpp>
# include <dismod_at/eigen_ode2.hpp>
# include <dismod_at/a1_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

namespace {
   // solution corresponding to b_1 = 0, b_2 = 0
   template <class Float>
   CppAD::vector<Float> both_zero(
      const CppAD::vector<Float>&  b           ,
      const CppAD::vector<Float>&  yi          ,
      const Float&                 tf          )
   {  using CppAD::exp;
      CppAD::vector<Float> yf(2);

      yf[0] = yi[0] * exp( b[0] * tf );
      yf[1] = yi[1] * exp( b[3] * tf );

      return yf;
   }
   // solution corresponding to b_1 = 0 , b_2 != 0
   template <class Float>
   CppAD::vector<Float> b1_zero(
      const CppAD::vector<Float>&  b           ,
      const CppAD::vector<Float>&  yi          ,
      const Float&                 tf          )
   {  using CppAD::exp;
      CppAD::vector<Float> yf(2);
      double eps    = std::numeric_limits<double>::epsilon();
      Float  small  = Float( std::sqrt(eps) );
      Float diff_03 = b[0] - b[3];
      //
      // y_0 ( tf )
      yf[0] = yi[0] * exp( b[0] * tf );
      //
      // exp[ (b0 - b3) * tf ] / (b0 - b3);
      Float term   = expm1( diff_03 * tf ) / diff_03;
      Float approx = tf  + diff_03 * tf * tf / Float(2.0);
      term = CppAD::CondExpLt(fabs(diff_03), small, approx, term);
      //
      // y_1 ( tf )
      yf[1] = exp( b[3] * tf ) * ( yi[1] + b[2] * yi[0] * term );
      //
      return yf;
   }
   // solution corresponding to b1 != 0 , b2 == 0
   template <class Float>
   CppAD::vector<Float> b2_zero(
      const CppAD::vector<Float>&  b           ,
      const CppAD::vector<Float>&  yi          ,
      const Float&                 tf          )
   {  using CppAD::exp;
      CppAD::vector<Float> yf(2);
      double eps    = std::numeric_limits<double>::epsilon();
      Float  small  = Float( std::sqrt(eps) );
      Float diff_30 = b[3] - b[0];
      //
      // y_1 ( tf )
      yf[1] = yi[1] * exp( b[3] * tf );
      //
      // exp[ (b3 - b0) * tf ] / (b3 - b0);
      Float term   = expm1( diff_30 * tf ) / diff_30;
      Float approx = tf  + diff_30 * tf * tf / Float(2.0);
      term = CppAD::CondExpLt(fabs(diff_30), small, approx, term);
      //
      // y_0 ( tf )
      yf[0] = exp( b[0] * tf ) * ( yi[0] + b[1] * yi[1] * term );
      //
      return yf;
   }
   // solution corresponding to b_1 != 0, b_2 != 0
   template <class Float>
   CppAD::vector<Float> both_nonzero(
      const CppAD::vector<Float>&  b           ,
      const CppAD::vector<Float>&  yi          ,
      const Float&                 tf          )
   {  using CppAD::exp;
      CppAD::vector<Float> yf(2);
      // discriminant in the quadratic equation for eigen-values
      Float disc = (b[0] - b[3])*(b[0] - b[3]) + 4.0*b[1]*b[2];
      Float root_disc = Float(sqrt( disc ));

      // use eigen vectors
      Float lambda_p  = (b[0] + b[3] + root_disc) / 2.0;
      Float lambda_m  = (b[0] + b[3] - root_disc) / 2.0;
      //
      Float u_p       = (lambda_p - b[0]) / b[2];
      Float u_m       = (lambda_m - b[0]) / b[2];
      //
      Float zi_p      = yi[0] + u_p * yi[1];
      Float zi_m      = yi[0] + u_m * yi[1];
      //
      Float zf_p      = zi_p * exp( lambda_p * tf );
      Float zf_m      = zi_m * exp( lambda_m * tf );
      //
      yf[1]           = (zf_p - zf_m) * b[2] / root_disc;
      yf[0]           = zf_p - u_p * yf[1];
      //
      return yf;
   }
}

template <class Float>
CppAD::vector<Float> eigen_ode2(
   size_t                       case_number ,
   const CppAD::vector<Float>&  b           ,
   const CppAD::vector<Float>&  yi          ,
   const Float&                 tf          )
{  using CppAD::abs;
   using CppAD::exp;
   using CppAD::sqrt;
   //
   assert( b.size() == 4 );
   assert( yi.size() == 2 );
   assert( 1 <= case_number && case_number <= 4 );
   CppAD::vector<Float> yf(2);

   // solution corresponding to b_1 = b_2 = 0
   if( case_number == 1 )
   {  return both_zero(b, yi, tf);
   }

   // case for which we switch the order of the rows and columns
   if( case_number == 2 )
      return b2_zero(b, yi, tf);
   //
   if( case_number == 3 )
      return b1_zero(b, yi, tf);
   //
   assert( case_number == 4 );
   return both_nonzero(b, yi, tf);
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_EIGEN_ODE2(Float)       \
   template CppAD::vector<Float> eigen_ode2<Float>(   \
      size_t                       case_number ,     \
      const CppAD::vector<Float>&  b           ,     \
      const CppAD::vector<Float>&  yi          ,     \
      const Float&                 tf                \
   );

// instantiations
DISMOD_AT_INSTANTIATE_EIGEN_ODE2( double )
DISMOD_AT_INSTANTIATE_EIGEN_ODE2( a1_double )

} // END DISMOD_AT_NAMESPACE
