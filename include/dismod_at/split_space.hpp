// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SPLIT_SPACE_HPP
# define DISMOD_AT_SPLIT_SPACE_HPP

# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
		CppAD::vector<std::string> split_space(const std::string& str);
}

# endif
