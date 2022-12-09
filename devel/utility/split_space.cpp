// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/split_space.hpp>
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
{xrst_begin split_space dev}
{xrst_spell
   delimiter
   substring
   substrings
}

Split String Into Substrings With Spaces as Delimiter
#####################################################

Syntax
******
``vec`` = *split_space* ( ``str`` )

str
***
This is the string that we are splitting.
Each substring is non-empty with by one or more spaces between them.

vec
***
This is the resulting vector of non-empty substrings.
If all the characters in *str* are spaces,
the resulting vector with have size zero.

empty
*****
If *str* only contains spaces,
the return vector is empty; i.e.
*vec* . ``size`` () == 0 .
{xrst_toc_hidden
   example/devel/utility/split_space_xam.cpp
}
Example
*******
The file :ref:`split_space_xam.cpp-name` contains an example and test
of using this routine.

Prototype
*********
{xrst_spell_off}
{xrst_code cpp} */
CppAD::vector<std::string> split_space(const std::string& str)
/* {xrst_code}
{xrst_spell_on}

{xrst_end split_space}
*/
{  CppAD::vector<std::string> result;
   size_t start = 0;
   while( start < str.size() )
   {  while( start < str.size() && str[start] == ' ' )
         ++start;
      if( start < str.size() )
      {  size_t stop = start + 1;
         while( stop < str.size() && str[stop] != ' ' )
            ++stop;
         std::string sub = str.substr(start, stop - start);
         result.push_back(sub);
         //
         start = stop;
      }
   }
   return result;
}

} // END_DISMOD_AT_NAMEPSACE
