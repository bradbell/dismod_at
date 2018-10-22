// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin split_space_xam.cpp$$
$spell
	xam
$$

$section C++ split_space: Example and Test$$

$srcfile%example/devel/utility/split_space_xam.cpp%0%// BEGIN C++%// END C++%1%$$

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
