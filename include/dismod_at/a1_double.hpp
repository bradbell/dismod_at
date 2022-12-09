// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_A1_DOUBLE_HPP
# define DISMOD_AT_A1_DOUBLE_HPP
# include <cppad/cppad.hpp>

/*
{xrst_begin a1_double}
{xrst_spell
   hpp
}

Declare a1_double Type
######################

Syntax
******
# ``include <dismod_at/a1_double.hpp>``

Source Code
***********
{xrst_spell_off}
{xrst_code cpp} */
namespace dismod_at {
   typedef CppAD::AD<double>  a1_double;
}
/* {xrst_code}
{xrst_spell_on}

{xrst_end a1_double}
*/

# endif
