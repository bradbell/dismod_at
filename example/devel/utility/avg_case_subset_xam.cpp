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
$begin avg_case_subset_xam.cpp$$
$spell
	avg
	xam
$$

$section C++ avg_case_subset: Example and Test$$

$code
$verbatim%example/devel/table/avg_case_subset_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/get_avg_case_table.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/avg_case_subset.hpp>

bool avg_case_subset_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;
	double inf = std::numeric_limits<double>::infinity();

	// covariate table
	size_t n_covariate = 2;
	vector<dismod_at::covariate_struct> covariate_table(n_covariate);
	covariate_table[0].reference      = -0.5; // sex reference value
	covariate_table[0].max_difference = 0.6;  // maximum sex difference
	covariate_table[1].reference      = 0.0;
	covariate_table[1].max_difference = inf;

	// node table
	size_t n_node = 4;
	vector<dismod_at::node_struct> node_table(n_node);
	node_table[0].node_name = "world";
	node_table[1].node_name = "north_america";
	node_table[2].node_name = "united_states";
	node_table[3].node_name = "canada";
	//
	node_table[0].parent = -1;
	node_table[1].parent = 0;
	node_table[2].parent = 1;
	node_table[3].parent = 1;

	// default value for a avg_case table record
	dismod_at::avg_case_struct record;
	record.integrand_id   = 1;
	record.weight_id      = 4;
	record.age_lower      = 10.0;
	record.age_upper      = 90.0;
	record.time_lower     = 2000.0;
	record.time_upper     = 2010.0;
	record.x.resize(n_covariate);

	// avg_case table
	size_t n_avg_case = n_node;
	vector<dismod_at::avg_case_struct> avg_case_table(n_avg_case);
	for(size_t avg_case_id = 0; avg_case_id < n_avg_case; avg_case_id++)
	{	avg_case_table[avg_case_id]         = record;
		avg_case_table[avg_case_id].node_id = avg_case_id;
		avg_case_table[avg_case_id].x[1]    = 100. * avg_case_id;
	}
	avg_case_table[0].x[0] = -0.5; // sex reference
	avg_case_table[1].x[0] = -0.5; // sex reference
	avg_case_table[2].x[0] = 0.0;  // within sex bounds
	avg_case_table[3].x[0] = 0.5;  // out of sex bounds
	//
	// child_object
	size_t parent_node_id = 1; // north_america
	dismod_at::child_info child_object(
		parent_node_id, node_table, avg_case_table
	);

	// avg_case_subset_obj
	vector<dismod_at::avg_case_subset_struct> avg_case_subset_obj =
		avg_case_subset(avg_case_table, covariate_table, child_object);

	// avg_case_id = 0 is for world and hence not included
	ok &= avg_case_subset_obj[0].original_id == 1;
	// avg_case_id = 1 covariate values minus corresponding reference value
	ok &= avg_case_subset_obj[0].x[0]    == 0.0;
	ok &= avg_case_subset_obj[0].x[1]    == 100.;
	ok &= avg_case_subset_obj[0].node_id == 1;
	// avg_case_id = 2 covariate values minus corresponding reference value
	ok &= avg_case_subset_obj[1].original_id == 2;
	ok &= avg_case_subset_obj[1].x[0]    == 0.5;
	ok &= avg_case_subset_obj[1].x[1]    == 200.;
	ok &= avg_case_subset_obj[1].node_id == 2;
	// avg_case_id = 3 is has a sex covariate that is out of bounds
	ok &= avg_case_subset_obj.size() == 2;

	return ok;
}
// END C++
