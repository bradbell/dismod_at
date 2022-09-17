// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_NULL_INT_HPP
# define DISMOD_AT_NULL_INT_HPP
# include <limits>
# define DISMOD_AT_NULL_INT     std::numeric_limits<int>::min()
# define DISMOD_AT_NULL_SIZE_T  size_t(DISMOD_AT_NULL_INT)
# endif
