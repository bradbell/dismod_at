// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin fit_model_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ pack_prior: Example and Test$$

$code
$verbatim%example/devel/model/fit_model_xam.cpp%0
	%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/fit_model.hpp>

bool fit_model_xam(void)
{	bool   ok = true;
	size_t i, j;
	using CppAD::vector;
	using std::cout;
	using std::endl;
	double inf = std::numeric_limits<double>::infinity();


	// --------------------------------------------------------------------
	// age_table = [ 0 , 20, 40, 60, 80, 100 ]
	size_t n_age_table = 6;
	vector<double> age_table(n_age_table);
	for(i = 0; i < n_age_table; i++)
		age_table[i] = 100. * i / double(n_age_table - 1);
	//
	// time_table = [ 1995, 2005, 2015 ]
	size_t n_time_table = 3;
	vector<double> time_table(n_time_table);
	for(i = 0; i < n_time_table; i++)
		time_table[i] = 1995. + (2015- 1995) * i / double(n_time_table-1);
	// ----------------------- prior table ---------------------------------
	size_t n_prior_table = 4;
	vector<dismod_at::prior_struct> prior_table(n_prior_table);
	//
	// prior_id_zero (identically zero prior)
	prior_table[0].prior_name = "zero";
	prior_table[0].density_id = int( dismod_at::uniform_enum );
	prior_table[0].lower      = 0.0;
	prior_table[0].mean       = 0.0;
	prior_table[0].upper      = 0.0;
	prior_table[0].std        = 1.0;
	//
	// prior_id_one (identically one prior)
	prior_table[1].prior_name = "one";
	prior_table[1].density_id = int( dismod_at::uniform_enum );
	prior_table[1].lower      = 1.0;
	prior_table[1].mean       = 1.0;
	prior_table[1].upper      = 1.0;
	prior_table[1].std        = 1.0;
	//
	size_t prior_id_one       = 1;
	//
	// prior_id_none (no prior)
	prior_table[2].prior_name = "none";
	prior_table[2].density_id = int( dismod_at::uniform_enum );
	prior_table[2].lower      = - inf;
	prior_table[2].mean       = 0.0;
	prior_table[2].upper      = + inf;
	prior_table[2].std        = 1.0;
	size_t prior_id_none      = 2;
	//
	// prior_id_gaussian (Gaussian mean 0.01 and standard deviation 0.002)
	prior_table[3].prior_name = "N(1e-2, 4e-6)";
	prior_table[3].density_id = int( dismod_at::gaussian_enum );
	prior_table[3].lower      = -inf;
	prior_table[3].mean       = 0.01;
	prior_table[3].upper      = +inf;
	prior_table[3].std        = 0.002;
	size_t prior_id_gaussian  = 3;
	// -------------------------------------------------------------------
	// smoothing information
	vector<size_t> age_id, time_id;
	vector<size_t> value_prior_id, dage_prior_id, dtime_prior_id;
	size_t mulstd_value, mulstd_dage, mulstd_dtime;
	size_t n_age, n_time, n_grid;

	vector<dismod_at::smooth_info> s_info_vec(4);
	// ------------------ first and second smoothing --------------------------
	// age_id
	n_age = 3;
	age_id.resize(n_age);
	age_id[0] = 0;
	age_id[1] = n_age_table / 2;
	age_id[2] = n_age_table - 1;
	//
	// time_id
	n_time = 2;
	time_id.resize(n_time);
	time_id[0] = 0;
	time_id[1] = n_time_table - 1;
	//
	// scalar prior_id
	mulstd_value = prior_id_one;
	mulstd_dage  = prior_id_one;
	mulstd_dtime = prior_id_one;
	//
	// prior_id values on an n_age * n_time grid
	n_grid = n_age * n_time;
	value_prior_id.resize(n_grid);
	dage_prior_id.resize(n_grid);
	dtime_prior_id.resize(n_grid);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	value_prior_id[ i * n_time + j ] = prior_id_gaussian;
			dage_prior_id[ i * n_time + j ]  = prior_id_none;
			dtime_prior_id[ i * n_time + j ] = prior_id_none;
		}
	}
	//
	// smooth_id_32_gaussian
	size_t smooth_id_32_gaussian = 0;
	s_info_vec[smooth_id_32_gaussian] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	//
	// smooth_id_32_none
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
			value_prior_id[ i * n_time + j ] = prior_id_none;
	}
	size_t smooth_id_32_none = 1;
	s_info_vec[smooth_id_32_none] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	// ------------------ third and fourth smoothing --------------------------
	// age_id
	n_age = 1;
	age_id.resize(n_age);
	age_id[0] = n_age_table / 2;
	//
	// time_id
	n_time = 2;
	time_id.resize(n_time);
	time_id[0] = 0;
	time_id[1] = n_time_table - 1;
	//
	// scalar prior_id
	mulstd_value = prior_id_one;
	mulstd_dage  = prior_id_one;
	mulstd_dtime = prior_id_one;
	//
	// prior_id valeus on an n_age * n_time grid
	n_grid = n_age * n_time;
	value_prior_id.resize(n_grid);
	dage_prior_id.resize(n_grid);
	dtime_prior_id.resize(n_grid);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	value_prior_id[ i * n_time + j ] = prior_id_gaussian;
			dage_prior_id[ i * n_time + j ]  = prior_id_none;
			dtime_prior_id[ i * n_time + j ] = prior_id_none;
		}
	}
	//
	// smooth_id_12_gaussian
	size_t smooth_id_12_gaussian = 2;
	s_info_vec[smooth_id_12_gaussian] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	//
	// smooth_id_12_none
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
			value_prior_id[ i * n_time + j ] = prior_id_none;
	}
	size_t smooth_id_12_none = 3;
	s_info_vec[smooth_id_12_none] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	// --------------------------------------------------------------------
	// smooth_table
	vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	smooth_table[smooth_id].n_age  = s_info_vec[smooth_id].age_size();
		smooth_table[smooth_id].n_time = s_info_vec[smooth_id].time_size();
	}
	//
	// mulcov_table
	vector<dismod_at::mulcov_struct> mulcov_table(0);
	//
	// rate_table
	vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	if( rate_id == dismod_at::pini_enum )
		{	// smoothing must have only one age
			rate_table[rate_id].parent_smooth_id = smooth_id_12_gaussian;
			rate_table[rate_id].child_smooth_id  = smooth_id_12_gaussian;
		}
		else
		{	// eventually plan to use smooth_id_32_gaussian here
			rate_table[rate_id].parent_smooth_id = smooth_id_12_gaussian;
			rate_table[rate_id].child_smooth_id  = smooth_id_12_gaussian;
		}
	}
	// change parent iota smoothing to be none
	// evantually plan tu use smooth_id_32_none here
	rate_table[dismod_at::iota_enum].parent_smooth_id = smooth_id_12_none;
	//
	// integrand_table
	size_t n_integrand = dismod_at::number_integrand_enum;
	vector<dismod_at::integrand_struct> integrand_table(n_integrand);
	for(i = 0; i < n_integrand; i++)
	{	integrand_table[i].integrand = dismod_at::integrand_enum(i);
		integrand_table[i].eta       = 1e-6;
	}
	//
	// node_table:    0
	//              1    2
	//                  3  4
	CppAD::vector<dismod_at::node_struct> node_table(5);
	node_table[0].parent = -1;
	node_table[1].parent =  0;
	node_table[2].parent =  0;
	node_table[3].parent =  2;
	node_table[4].parent =  2;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// pack_object
	size_t n_child     = 2;
	dismod_at::pack_info pack_object(
		n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	//
	// w_info_vec (the constant one)
	vector<double> weight(1);
	weight[0] = 1.0;
	age_id.resize(1);
	time_id.resize(1);
	age_id[0] = 0;
	time_id[0] = 0;
	dismod_at::weight_info w_info(age_id, time_id, weight);
	vector<dismod_at::weight_info> w_info_vec(1);
	w_info_vec[0] = w_info;
	//
	// data_table
	vector<dismod_at::data_struct> data_table(1);
	size_t data_id = 0;
	vector<double> x(0);
	data_table[data_id].integrand_id = dismod_at::mtother_enum;
	data_table[data_id].node_id      = 0;
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = 50.0;
	data_table[data_id].age_upper    = 50.0;
	data_table[data_id].time_lower   = 2000.0;
	data_table[data_id].time_upper   = 2000.0;
	data_table[data_id].meas_value   = 3e-2;
	data_table[data_id].meas_std     = 3e-3;
	data_table[data_id].density_id   = dismod_at::gaussian_enum;
	data_table[data_id].x            = x;
	//
	// prior_object
	dismod_at::prior_model prior_object(
		pack_object, age_table, time_table, prior_table, s_info_vec
	);
	//
	// data_model
	size_t n_age_ode = 6;
	size_t n_time_ode = 2;
	double ode_step_size = 20.;
	dismod_at::data_model data_object(
		parent_node_id,
		n_age_ode,
		n_time_ode,
		ode_step_size,
		age_table,
		time_table,
		integrand_table,
		node_table,
		data_table,
		w_info_vec,
		s_info_vec,
		pack_object
	);
	// ----------------------- run the fit -------------------------------
	dismod_at::fit_model fit_object(
		pack_object,
		prior_table,
		s_info_vec,
		data_object,
		prior_object
	);
	fit_object.run_fit();
	return ok;
}
// END C++
