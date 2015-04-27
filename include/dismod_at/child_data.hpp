// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHILD_DATA_HPP
# define DISMOD_AT_CHILD_DATA_HPP

# include <cppad/cppad.hpp>
# include "get_node_table.hpp"
# include "get_data_table.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

class child_data {
private:
	CppAD::vector<size_t> child_id2node_id_;
	CppAD::vector<size_t> data_id2child_;
public:
	child_data(
		size_t                            parent_node_id ,
		const CppAD::vector<node_struct>& node_table     ,
		const CppAD::vector<data_struct>& data_table
	);
	size_t child_size(void) const;
	size_t child_id2node_id(size_t child_id) const;
	size_t data_id2child(size_t data_id) const;
};

} // END DISMOD_AT_NAMESPACE

# endif
