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
$begin child_info_xam.cpp$$
$spell
	var
$$

$section C++ child_info: Example and Test$$
$index example, C++ child_info$$
$index child_info, C++ example$$

$code
$verbatim%example/devel/utility/child_info_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_data_table.hpp>

bool child_info_xam(void)
{	bool ok = true;
	using CppAD::vector;

	// parent node_id
	size_t parent_node_id = 0;

	// node_table
	size_t n_node = 4;
	CppAD::vector<dismod_at::node_struct> node_table(n_node);
	node_table[0].parent = -1; // parent node
	node_table[1].parent =  0; // child_id = 0
	node_table[2].parent =  0; // child_id = 1
	node_table[3].parent =  2; // grand child node below child_id = 2

	// data_table
	size_t n_data = 4;
	CppAD::vector<dismod_at::data_struct> data_table(n_data);
	data_table[0].node_id = 3; // in child_id = 1  set
	data_table[1].node_id = 2; // in child_id = 1  set
	data_table[2].node_id = 1; // in child_id = 0  set
	data_table[3].node_id = 0; // in parent node   set

	// constructor
	dismod_at::child_info child_object(
		parent_node_id, node_table, data_table
	);

	// check child_size
	size_t n_child = child_object.child_size();
	ok &= n_child == 2;

	// check child_id2node_id
	ok &= child_object.child_id2node_id(0) == 1; // first  child node_id = 1
	ok &= child_object.child_id2node_id(1) == 2; // second child node_id = 2

	// check other_id2child_id
	ok &= child_object.other_id2child(0) == 1;      // first  data child_id = 1
	ok &= child_object.other_id2child(1) == 1;      // second data child_id = 1
	ok &= child_object.other_id2child(2) == 0;      // third  data child_id = 0
	ok &= child_object.other_id2child(3) == n_child;// fourth in parent set

	return ok;
}
// END C++
