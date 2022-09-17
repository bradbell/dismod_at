// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHILD_INFO_HPP
# define DISMOD_AT_CHILD_INFO_HPP

# include <cppad/cppad.hpp>
# include "get_node_table.hpp"

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

class child_info {
private:
	CppAD::vector<size_t> child_id2node_id_;
	CppAD::vector<size_t> table_id2child_;
public:
	template <class Row>
	child_info(
		size_t                                parent_node_id ,
		const CppAD::vector<node_struct>&     node_table     ,
		const CppAD::vector<Row>&             table
	);
	size_t child_size(void) const;
	size_t child_id2node_id(size_t child_id) const;
	size_t table_id2child(size_t table_id) const;
};

} // END DISMOD_AT_NAMESPACE

# endif
