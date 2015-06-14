// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin to_string_xam.cpp$$
$spell
	xam
$$

$section C++ to_string: Example and Test$$

$code
$verbatim%example/devel/utility/to_string_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/to_string.hpp>

bool to_string_xam(void)
{	bool ok = true;
	//
	std::string s = dismod_at::to_string(101);
	ok &= s == "101";

	return ok;
}
// END C++
