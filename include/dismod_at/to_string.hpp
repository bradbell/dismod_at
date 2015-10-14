// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_TO_STRING_HPP
# define DISMOD_AT_TO_STRING_HPP
/*
$begin to_string$$
$spell
	const
	std
$$

$section Convert to a std::string$$

$head Syntax$$
$icode%s% = to_string(%v%)%$$

$head v$$
This argument has prototype
$codei%
	const %Value%& %v%
%$$
where the type $icode Value$$ is such that
the output stream operator $codei%<< %v%$$ defined.

$head s$$
The return value has prototype
$codei%
	std::string %s%
%$$
It contains a string representation of the value $icode v$$.

$children%example/devel/utility/to_string_xam.cpp
%$$
$head Example$$
The file $cref to_string_xam.cpp$$ contains an example and test
of using this routine.

$end
------------------------------------------------------------------------------
*/

# include <string>
# include <sstream>
# include <cmath>
# include <iomanip>
# include <limits>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
	template <class Value>
	std::string to_string(const Value& v)
	{	std::ostringstream os;
		os << v;
		return os.str();
	}
	inline std::string to_string(const double& v)
	{	std::ostringstream os;
		int digits = 1 - std::log10( std::numeric_limits<double>::epsilon() );
		os << std::setprecision(digits) << v;
		return os.str();
	}
} // END_DISMOD_AT_NAMESPACE


# endif
