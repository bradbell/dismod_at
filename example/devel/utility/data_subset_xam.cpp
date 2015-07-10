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
$begin data_subset_xam.cpp$$
$spell
	data
	xam
$$

$section C++ data_subset: Example and Test$$

$code
$verbatim%example/devel/table/data_subset_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/data_subset.hpp>

bool data_subset_xam(void)
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

	// default value for a data table record
	dismod_at::data_struct record;
	record.integrand_id   = 1;
	record.density_id     = 0;
	record.weight_id      = 4;
	record.meas_value     = 1e-4;
	record.meas_std       = 1e-5;
	record.age_lower      = 10.0;
	record.age_upper      = 90.0;
	record.time_lower     = 2000.0;
	record.time_upper     = 2010.0;
	record.x.resize(n_covariate);

	// data table
	size_t n_data = n_node;
	vector<dismod_at::data_struct> data_table(n_data);
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	data_table[data_id]         = record;
		data_table[data_id].node_id = data_id;
		data_table[data_id].x[1]    = 100. * data_id;
	}
	data_table[0].x[0] = -0.5; // sex reference
	data_table[1].x[0] = -0.5; // sex reference
	data_table[2].x[0] = 0.0;  // within sex bounds
	data_table[3].x[0] = 0.5;  // out of sex bounds
	//
	// child_object
	size_t parent_node_id = 1; // north_america
	dismod_at::child_info child_object(
		parent_node_id, node_table, data_table
	);

	// data_subset_obj
	vector<dismod_at::data_subset_struct> data_subset_obj =
		data_subset(data_table, covariate_table, child_object);

	// data_id = 0 is for world and hence not included
	ok &= data_subset_obj[0].original_id == 1;
	// data_id = 1 covariate values minus corresponding reference value
	ok &= data_subset_obj[0].x[0]    == 0.0;
	ok &= data_subset_obj[0].x[1]    == 100.;
	ok &= data_subset_obj[0].node_id == 1;
	// data_id = 2 covariate values minus corresponding reference value
	ok &= data_subset_obj[1].original_id == 2;
	ok &= data_subset_obj[1].x[0]    == 0.5;
	ok &= data_subset_obj[1].x[1]    == 200.;
	ok &= data_subset_obj[1].node_id == 2;
	// data_id = 3 is has a sex covariate that is out of bounds
	ok &= data_subset_obj.size() == 2;

	return ok;
}
// END C++
