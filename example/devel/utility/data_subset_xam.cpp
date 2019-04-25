// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
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

$srcfile%example/devel/utility/data_subset_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/data_subset.hpp>
# include <dismod_at/null_int.hpp>

bool data_subset_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;
	double inf = std::numeric_limits<double>::infinity();
	//
	// density_table
	vector<dismod_at::density_enum> density_table(7);
	density_table[0] = dismod_at::uniform_enum;
	density_table[1] = dismod_at::gaussian_enum;
	density_table[2] = dismod_at::laplace_enum;
	density_table[3] = dismod_at::students_enum;
	density_table[4] = dismod_at::log_gaussian_enum;
	density_table[5] = dismod_at::log_laplace_enum;
	density_table[6] = dismod_at::log_students_enum;

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
	node_table[0].parent = DISMOD_AT_NULL_INT;
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

	// data table
	size_t n_data = n_node;
	vector<dismod_at::data_struct> data_table(n_data);
	vector<double> data_cov_value( n_data * n_covariate );
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	data_table[data_id]         = record;
		data_table[data_id].node_id =  int( data_id );
		data_cov_value[ data_id * n_covariate + 1 ] = 100. * double(data_id);
	}
	data_cov_value[ 0 * n_covariate + 0] = -0.5; // sex reference
	data_cov_value[ 1 * n_covariate + 0] = -0.5; // sex reference
	data_cov_value[ 2 * n_covariate + 0] =  0.0; // within sex bounds
	data_cov_value[ 3 * n_covariate + 0] =  0.5; // out of sex bounds
	//
	// child_object
	size_t parent_node_id = 1; // north_america
	dismod_at::child_info child_object(
		parent_node_id, node_table, data_table
	);

	// data_subset_obj
	vector<dismod_at::data_subset_struct> data_subset_obj;
	vector<double> data_subset_cov_value;
	data_subset(
		density_table,
		data_table,
		data_cov_value,
		covariate_table,
		child_object,
		data_subset_obj,
		data_subset_cov_value
	);

	// data_id = 0 is for world and hence not included
	ok &= data_subset_obj[0].original_id == 1;
	// data_id = 1 covariate values minus corresponding reference value
	ok &= data_subset_cov_value[0 * n_covariate + 0]    == 0.0;
	ok &= data_subset_cov_value[0 * n_covariate + 1]    == 100.;
	ok &= data_subset_obj[0].node_id == 1;
	// data_id = 2 covariate values minus corresponding reference value
	ok &= data_subset_obj[1].original_id == 2;
	ok &= data_subset_cov_value[1 * n_covariate + 0]    == 0.5;
	ok &= data_subset_cov_value[1 * n_covariate + 1]    == 200.;
	ok &= data_subset_obj[1].node_id == 2;
	// data_id = 3 is has a sex covariate that is out of bounds
	ok &= data_subset_obj.size() == 2;

	return ok;
}
// END C++
