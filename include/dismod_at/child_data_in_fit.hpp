// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHILD_DATA_IN_FIT_HPP
# define DISMOD_AT_CHILD_DATA_IN_FIT_HPP

# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	CppAD::vector<size_t> child_data_in_fit(
		const std::map<std::string, std::string>&    option_map            ,
		const CppAD::vector<data_subset_struct>&     data_subset_table     ,
		const CppAD::vector<integrand_struct>&       integrand_table       ,
		const CppAD::vector<data_struct>&            data_table            ,
		const child_info&                            child_info4data
	);
}

# endif
