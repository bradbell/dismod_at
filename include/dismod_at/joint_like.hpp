// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_JOINT_LIKE_HPP
# define DISMOD_AT_JOINT_LIKE_HPP

# include <limits>
# include <cppad/vector.hpp>
# include "data_model.hpp"
# include "pack_info.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class joint_like {
private:
	const data_model&                  data_object_;
	const pack_info&                    pack_object_;
	const CppAD::vector<prior_struct>& prior_table_;
public:
	data_model(
		const data_model&                  data_object ,
		const pack_info&                    pack_object   ,
		const CppAD::vector<prior_struct>& prior_table
	);
};

} // END_DISMOD_AT_NAMESPACE

# endif
