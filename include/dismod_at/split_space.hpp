// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_SPLIT_SPACE_HPP
# define DISMOD_AT_SPLIT_SPACE_HPP

# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
		CppAD::vector<std::string> split_space(const std::string& str);
}

# endif
