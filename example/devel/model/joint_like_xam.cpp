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
$begin joint_like_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ joint_like: Example and Test$$

$code
$verbatim%example/devel/model/joint_like_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <cmath>
# include <dismod_at/prior_density.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/joint_like.hpp>

namespace {
	bool res_equal(
		const dismod_at::residual_struct<double>& left  ,
		const dismod_at::residual_struct<double>& right )
	{	bool result = left.wres == right.wres;
		result     &= left.logden_smooth == right.logden_smooth;
		result     &= left.logden_sub_abs == right.logden_sub_abs;
		result     &= left.density == right.density;
		return result;
	}
}

bool joint_like_xam(void)
{	bool   ok = true;
	size_t i, j, k;
	using CppAD::vector;
	using std::cout;
	using std::endl;
	double inf = std::numeric_limits<double>::infinity();
	//
	// ode_step_size
	double ode_step_size = 30.0;
	// --------------------------------------------------------------------
	// age_table
	size_t n_age_table = 10;
	vector<double> age_table(n_age_table);
	for(i = 0; i < n_age_table; i++)
		age_table[i] = 100 * i / double(n_age_table - 1);
	double age_min = age_table[0];
	double age_max = age_table[n_age_table-1];
	//
	// time_table
	size_t n_time_table = 6;
	vector<double> time_table(n_time_table);
	for(i = 0; i < n_time_table; i++)
		time_table[i] = 1975 + (2015 - 1975) * i / double(n_time_table - 1);
	double time_min = time_table[0];
	double time_max = time_table[n_time_table-1];
	//
	// integrand_table
	size_t n_integrand = dismod_at::number_integrand_enum;
	vector<dismod_at::integrand_struct> integrand_table(n_integrand);
	for(i = 0; i < n_integrand; i++)
	{	integrand_table[i].integrand = dismod_at::integrand_enum(i);
		integrand_table[i].eta       = 1e-4;
	}
	//
	// n_age_ode
	size_t n_age_ode     =  1;
	while( age_min + (n_age_ode-1) * ode_step_size < age_max )
			n_age_ode++;
	//
	// n_time_ode
	size_t n_time_ode     =  1;
	while( time_min + (n_time_ode-1) * ode_step_size < time_max )
			n_time_ode++;
	//
	// node_table:
	CppAD::vector<dismod_at::node_struct> node_table(1);
	node_table[0].parent = -1;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// data_table
	vector<dismod_at::data_struct> data_table(2);
	//
	// parent node, time and age integrantion.
	vector<double> x(0);
	for(size_t data_id = 0; data_id < 2; data_id++)
	{
		data_table[data_id].integrand_id = dismod_at::mtother_enum;
		data_table[data_id].node_id    = 0;
		data_table[data_id].weight_id  = 0;
		data_table[data_id].age_lower  = 35.0;
		data_table[data_id].age_upper  = 55.0;
		data_table[data_id].time_lower = 1990.0;
		data_table[data_id].time_upper = 2000.0;
		data_table[data_id].meas_value = ( 50. * 1995) / (100.0 * 2000.0);
		data_table[data_id].meas_std   = data_table[data_id].meas_value / 10.;
		if( data_id == 0 )
			data_table[data_id].density_id = dismod_at::gaussian_enum;
		else
			data_table[data_id].density_id = dismod_at::log_gaussian_enum;
		data_table[data_id].x          = x;
	}
	// ----------------------- prior table ---------------------------------
	size_t n_prior_table = 6;
	vector<dismod_at::prior_struct> prior_table(n_prior_table);
	// prior_id = 0
	prior_table[0].density_id = int( dismod_at::uniform_enum );
	prior_table[0].lower      = 0.0;
	prior_table[0].mean       = 0.0;
	prior_table[0].upper      = 0.0;
	// prior_id = 1
	prior_table[1].density_id = int( dismod_at::uniform_enum );
	prior_table[1].lower      = 1.0;
	prior_table[1].mean       = 1.0;
	prior_table[1].upper      = 1.0;
	// size_t prior_id_none = 2;
	prior_table[2].density_id = int( dismod_at::uniform_enum );
	prior_table[2].lower      = -inf;
	prior_table[2].mean       = 0.0;
	prior_table[2].upper      = +inf;
	size_t prior_id_gaussian = 3;
	prior_table[3].density_id = int( dismod_at::gaussian_enum );
	prior_table[3].lower      = - 1.0;
	prior_table[3].mean       = 0.0;
	prior_table[3].upper      = 1.0;
	prior_table[3].std        = 0.5;
	size_t prior_id_laplace = 4;
	prior_table[4].density_id = int( dismod_at::laplace_enum );
	prior_table[4].lower      = -1.0;
	prior_table[4].mean       = 0.0;
	prior_table[4].upper      = 1.0;
	prior_table[4].std        = 0.5;
	size_t prior_id_log_gaussian = 5;
	prior_table[5].density_id = int( dismod_at::log_gaussian_enum );
	prior_table[5].lower      = 0.01;
	prior_table[5].mean       = 0.1;
	prior_table[5].upper      = 1.0;
	prior_table[5].std        = 0.5;
	prior_table[5].eta        = 1e-3;
	// -------------------------------------------------------------------
	// smoothing information
	vector<size_t> age_id, time_id;
	vector<size_t> value_prior_id, dage_prior_id, dtime_prior_id;
	size_t mulstd_value, mulstd_dage, mulstd_dtime;
	size_t n_age, n_time, n_grid;

	vector<dismod_at::smooth_info> s_info_vec(2);
	// ------------------ first smoothing ------------------------------------
	// age_id
	n_age = 3;
	age_id.resize(n_age);
	age_id[0] = 0;
	age_id[1] = n_age_table / 2;
	age_id[2] = n_age_table - 1;
	// time_id
	n_time = 2;
	time_id.resize(n_time);
	time_id[0] = 0;
	time_id[1] = n_time_table - 1;
	// scalar prior_id
	mulstd_value = 1;
	mulstd_dage  = 1;
	mulstd_dtime = 1;
	// grid prior ids
	n_grid = n_age * n_time;
	value_prior_id.resize(n_grid);
	dage_prior_id.resize(n_grid);
	dtime_prior_id.resize(n_grid);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	value_prior_id[ i * n_time + j ] = prior_id_gaussian;
			dage_prior_id[ i * n_time + j ]  = prior_id_laplace;
			dtime_prior_id[ i * n_time + j ] = prior_id_log_gaussian;
		}
	}
	//
	// s_info
	size_t smooth_id_3_by_2 = 0;
	s_info_vec[0] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	//
	// w_info_vec
	// weight value should not matter when constant
	size_t n_si = n_age * n_time;
	vector<double> weight(n_si);
	for(k = 0; k < n_si; k++)
		weight[k] = 0.5;
	dismod_at::weight_info w_info_0(age_id, time_id, weight);
	vector<dismod_at::weight_info> w_info_vec;
	w_info_vec.push_back( w_info_0 );
	// ------------------ second smoothing -----------------------------------
	// age_id
	n_age = 1;
	age_id.resize(n_age);
	age_id[0] = n_age_table / 2;
	// time_id
	n_time = 2;
	time_id.resize(n_time);
	time_id[0] = 0;
	time_id[1] = n_time_table - 1;
	// scalar prior_id
	mulstd_value = 1;
	mulstd_dage  = 1;
	mulstd_dtime = 1;
	// grid prior ids
	n_grid = n_age * n_time;
	value_prior_id.resize(n_grid);
	dage_prior_id.resize(n_grid);
	dtime_prior_id.resize(n_grid);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	value_prior_id[ i * n_time + j ] = prior_id_gaussian;
			dage_prior_id[ i * n_time + j ]  = prior_id_laplace;
			dtime_prior_id[ i * n_time + j ] = prior_id_log_gaussian;
		}
	}
	//
	// s_info
	size_t smooth_id_1_by_2 = 1;
	s_info_vec[1] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	//
	// w_info_vec
	// weight value should not matter when constant
	n_si = n_age * n_time;
	weight.resize(n_si);
	for(k = 0; k < n_si; k++)
		weight[k] = 0.5;
	dismod_at::weight_info w_info_1(age_id, time_id, weight);
	w_info_vec.push_back( w_info_1 );
	// ----------------------- pack_object --------------------------------
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
			rate_table[rate_id].parent_smooth_id = smooth_id_1_by_2;
			rate_table[rate_id].child_smooth_id  = smooth_id_1_by_2;
		}
		else
		{	rate_table[rate_id].parent_smooth_id = smooth_id_3_by_2;
			rate_table[rate_id].child_smooth_id  = smooth_id_3_by_2;
		}
	}
	// pack_object
	size_t n_child     = 1;
	dismod_at::pack_info pack_object(
		n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	// ----------------------------------------------------------------------
	// data_model
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
	// ----------------------- pack_vec -------------------------------------
	vector<double> pack_vec( pack_object.size() );
	dismod_at::pack_info::subvec_info info;
	//
	// mulstd
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	size_t offset  = pack_object.mulstd_offset(smooth_id);
		for(i = 0; i < 3; i++)
			pack_vec[offset + i] = 1.0;
	}
	//
	// rates
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	info = pack_object.rate_info(rate_id, child_id);
			dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
			n_age  = s_info.age_size();
			n_time = s_info.time_size();
			for(i = 0; i < n_age; i++)
			{	double age = age_table[ s_info.age_id(i) ];
				for(j = 0; j < n_time; j++)
				{	double time = time_table[ s_info.time_id(j) ];
					size_t index   = info.offset + i * n_time + j;
					pack_vec[index] = age * time / (age_max * time_max);
				}
			}
		}
	}
	//
	// meas_mean_mulcov: none
	// meas_std_mulcov: none
	// rate_mean_mulcov: none
	dismod_at::prior_density prior_object(
		pack_object, age_table, time_table, prior_table, s_info_vec
	);
	// -------------- joint likelihood -------------------------------------
	size_t n_fixed  = dismod_at::size_fixed_effect(pack_object);
	size_t n_random = dismod_at::size_random_effect(pack_object);
	CppAD::vector<double> fixed_vec(n_fixed), random_vec(n_random);
	dismod_at::put_fixed_effect(pack_object, pack_vec, fixed_vec);
	dismod_at::put_random_effect(pack_object, pack_vec, random_vec);
	//
	dismod_at::joint_like joint_object(pack_object, data_object, prior_object);
	CppAD::vector< dismod_at::residual_struct<double> > 
		joint_residual_vec = joint_object.eval(fixed_vec, random_vec);
	// -------------- check -------------------------------------------------
	CppAD::vector< dismod_at::residual_struct<double> > 
		prior_residual_vec = prior_object.eval(pack_vec);
	CppAD::vector< dismod_at::residual_struct<double> > 
		data_residual_vec = data_object.like_all(pack_vec);
	//
	size_t n_prior = prior_residual_vec.size();
	size_t n_data  = data_residual_vec.size();
	ok &= joint_residual_vec.size() == n_prior + n_data;
	// use unspecified fact that prior comes first 
	for(i = 0; i < n_prior; i++)
		ok &= res_equal( joint_residual_vec[i], prior_residual_vec[i] );
	for(i = 0; i < n_data; i++)
		ok &= res_equal( joint_residual_vec[i+n_prior], data_residual_vec[i] );
	
	return ok;
}
// END C++
