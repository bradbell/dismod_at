// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin trap_ode2}
{xrst_spell
   cc
   tf
   trapezoidal
   yf
   yi
}

Trapezoidal Solution of ODE with Two Components
###############################################

Syntax
******

   *yf* = ``trap_ode2`` ( *case_number* , *b* , *yi* , *tf* )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

Purpose
*******
Given :math:`y( 0 ) \in \B{R}^2`,
:math:`B \in \B{R}^{2 \times 2}`,
and :math:`t_f \in \B{R}_+`,
this routine uses the trapezoidal method vectors to solve for
:math:`y( t_f )` where

.. math::

   y' (t) = B \; y(t)

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

b
*
This vector has size four and specifies the matrix :math:`B` in
row major order; i.e.,

.. math::

   B
   =
   \left( \begin{array}{cc}
      b_0  & b_1 \\
      b_2  & b_3
   \end{array} \right)

yi
**
We use *yi* and :math:`y^i` to denote the initial value
:math:`y(0)`.
This vector has size two.

tf
**
We use *tf* and :math:`t_f` to denote the final time.

yf
**
We use *yf* and :math:`y^f` to denote the approximation
for :math:`y( t_f )`.
This vector has size two.
The trapezoidal method solves the implicit equation

.. math::

   y^f = y^i + t_f \; ( B y^i + B y^f  ) / 2

Method
******
The equation for :math:`y^f` can be written as

.. math::

   ( \R{I} - B t_f / 2 ) y^f =  ( \R{I} + B t_f / 2 ) y^i

where :math:`\R{I}` is the identify matrix.
This equation is solved using Cramer's rules so that
the set of floating point operations
does not depend on the argument values.
{xrst_toc_hidden
   example/devel/utility/trap_ode2_xam.cpp
}
Example
*******
The file :ref:`trap_ode2_xam.cpp-name` contains
and example and test of ``trap_ode2`` .
It returns true for success and false for failure.

{xrst_end trap_ode2}
---------------------------------------------------------------------------
*/
# include <cppad/cppad.hpp>
# include <dismod_at/trap_ode2.hpp>
# include <dismod_at/a1_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
template <class Float>
CppAD::vector<Float> trap_ode2(
   const CppAD::vector<Float>&  b           ,
   const CppAD::vector<Float>&  yi          ,
   const Float&                 tf          )
// END_PROTOTYPE
{  using CppAD::abs;
   using CppAD::exp;
   using CppAD::sqrt;
   //
   assert( b.size() == 4 );
   assert( yi.size() == 2 );
   //
   // tf2
   Float tf2 = tf / Float(2.0);
   //
   //  C = I - B * tf / 2
   Float c_0 = Float(1.0) - b[0] * tf2;
   Float c_1 =            - b[1] * tf2;
   Float c_2 =            - b[2] * tf2;
   Float c_3 = Float(1.0) - b[3] * tf2;
   //
   // x = (I + B * tf / 2) yi
   Float x_0 = yi[0] + (b[0] * yi[0] + b[1] * yi[1]) * tf2;
   Float x_1 = yi[1] + (b[2] * yi[0] + b[3] * yi[1]) * tf2;
   //
   // det_C = | c_0  c_1 |
   //         | c_2  c_3 |
   Float det_C = c_0 * c_3 - c_1 * c_2;
   //
   // yf
   CppAD::vector<Float> yf(2);
   //
   // yf[0] = | x_0 c_1 |
   //         | x_1 c_3 | / det_C
   yf[0] = (x_0 * c_3 - c_1 * x_1) / det_C;
   //
   // yf[1] = | c_0 x_0 |
   //         | c_2 x_1 | / det_C
   yf[1] = (c_0 * x_1 - x_0 * c_2) / det_C;
   //
   return yf;

}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_TRAP_ODE2(Float)       \
   template CppAD::vector<Float> trap_ode2<Float>(   \
      const CppAD::vector<Float>&  b           ,     \
      const CppAD::vector<Float>&  yi          ,     \
      const Float&                 tf                \
   );

// instantiations
DISMOD_AT_INSTANTIATE_TRAP_ODE2( double )
DISMOD_AT_INSTANTIATE_TRAP_ODE2( a1_double )

} // END DISMOD_AT_NAMESPACE
