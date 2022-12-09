// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin split_space_xam.cpp dev}

C++ split_space: Example and Test
#################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end split_space_xam.cpp}
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
