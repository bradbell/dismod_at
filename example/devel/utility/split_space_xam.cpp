// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin split_space_xam.cpp$$
$spell
   xam
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ split_space: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/split_space.hpp>

bool split_space_xam(void)
{
   bool   ok = true;

   std::string str = " one two  three  ";
   CppAD::vector<std::string> vec = dismod_at::split_space(str);

   ok &= vec.size() == 3;
   ok &= vec[0] == "one";
   ok &= vec[1] == "two";
   ok &= vec[2] == "three";

   return ok;
}
// END C++
