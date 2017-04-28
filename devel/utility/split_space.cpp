// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/split_space.hpp>
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
$begin split_space$$
$spell
	Substrings
	vec
	str
$$

$section Split String Into Substrings With Spaces as Delimiter$$

$head Syntax$$
$codei%vec% = split_space(%str%)%$$

$head str$$
This is the string that we are splitting.
Each substring is non-empty with by one or more spaces between them.

$head vec$$
This is the resulting vector of non-empty substrings.
If all the characters in $icode str$$ are spaces,
the resulting vector with have size zero.


$children%example/devel/utility/split_space_xam.cpp
%$$
$head Example$$
The file $cref split_space_xam.cpp$$ contains an example and test
of using this routine.

$head Prototype$$
$srccode%cpp% */
CppAD::vector<std::string> split_space(const std::string& str)
/* %$$
$end
*/
{	CppAD::vector<std::string> result;
	size_t start = 0;
	while( start < str.size() )
	{	while( start < str.size() && str[start] == ' ' )
			++start;
		if( start < str.size() )
		{	size_t stop = start + 1;
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
