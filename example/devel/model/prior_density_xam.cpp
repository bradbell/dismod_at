// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin prior_density_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ prior_density: Example and Test$$
$index example, C++ prior_density$$
$index prior_density, C++ example$$

$code
$verbatim%example/devel/model/prior_density_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/include/prior_density.hpp>
# include <dismod_at/include/get_density_table.hpp>

bool prior_density_xam(void)
{	bool   ok = true;
	size_t i, j;
	using CppAD::vector;	
	using std::cout;
	typedef CppAD::AD<double> Float;
	Float eps = CppAD::numeric_limits<Float>::epsilon() * 100;

	// --------------------------------------------------------------------
	// age_table 
	size_t n_age_table = 10;
	vector<double> age_table(n_age_table);
	for(i = 0; i < n_age_table; i++)
		age_table[i] = 100 * i / double(n_age_table - 1);
	//
	// time_table 
	size_t n_time_table = 5;
	vector<double> time_table(n_time_table);
	for(i = 0; i < n_time_table; i++)
		time_table[i] = (2015 - 1975) * i / double(n_time_table - 1);
	// ----------------------- prior table ---------------------------------
	size_t n_prior_table = 4;
	vector<dismod_at::prior_struct> prior_table(n_prior_table);
	// prior_id = 0
	prior_table[0].density_id = int( dismod_at::uniform_enum );
	prior_table[0].lower      = 0.0;
	prior_table[0].upper      = 0.0;
	// prior_id = 1
	prior_table[1].density_id = int( dismod_at::uniform_enum );
	prior_table[1].lower      = 1.0;
	prior_table[1].upper      = 1.0;
	size_t prior_id_gaussian = 2;
	prior_table[2].density_id = int( dismod_at::gaussian_enum );
	prior_table[2].lower      = - 1.0;
	prior_table[2].upper      = 1.0;
	prior_table[2].std        = 0.5;
	size_t prior_id_log_laplace = 3;
	prior_table[3].density_id = int( dismod_at::log_laplace_enum );
	prior_table[3].lower      = 0.0;
	prior_table[3].upper      = 0.1;
	prior_table[3].std        = 0.01;
	prior_table[3].eta        = 0.001;
	// -------------------------------------------------------------------
	// smoothing information structurs
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
			dage_prior_id[ i * n_time + j ]  = prior_id_gaussian;
			dtime_prior_id[ i * n_time + j ] = prior_id_gaussian;
		}
	}
	//
	// s_info
	size_t smooth_id_gaussian = 0;
	s_info_vec[0] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
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
		{	value_prior_id[ i * n_time + j ] = prior_id_log_laplace;
			dage_prior_id[ i * n_time + j ]  = prior_id_log_laplace;
			dtime_prior_id[ i * n_time + j ] = prior_id_log_laplace;
		}
	}
	//
	// s_info
	size_t smooth_id_log_laplace = 1;
	s_info_vec[1] = dismod_at::smooth_info(
			age_id, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
			mulstd_value, mulstd_dage, mulstd_dtime
	);
	// ----------------------- var_info --------------------------------
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
			rate_table[rate_id].parent_smooth_id = smooth_id_log_laplace;
			rate_table[rate_id].child_smooth_id  = smooth_id_log_laplace;
		}
		else
		{	rate_table[rate_id].parent_smooth_id = smooth_id_gaussian;
			rate_table[rate_id].child_smooth_id  = smooth_id_gaussian;
		}
	}
	// var_info
	size_t n_integrand = 0;
	size_t n_child     = 1;
	dismod_at::pack_var var_info(
		n_integrand, n_child, 
		smooth_table, mulcov_table, rate_table
	);
	// ----------------------- var_vec -------------------------------------
	vector<double> var_vec( var_info.size() );
	dismod_at::pack_var::subvec_info info;
	//
	// mulstd
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	size_t offset  = var_info.mulstd_offset(smooth_id);
		for(i = 0; i < 3; i++)
			var_vec[offset + i] = 1.0;
	}
	//
	// rates
	double age_max  = age_table[n_age_table - 1];
	double time_max = time_table[n_time_table - 1];
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	info = var_info.rate_info(rate_id, child_id);
			dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
			for(i = 0; i < s_info.age_size(); i++)
			{	double age = age_table[ s_info.age_id(i) ];
				for(j = 0; j < s_info.time_size(); j++)
				{	double time    = time_table[ s_info.time_id(j) ];
					size_t index   = info.offset + i * s_info.time_size() + j; 
					var_vec[index] = age * time / (age_max * time_max);
				}
			}
		}
	}
	//
	// meas_mean_mulcov: none
	// meas_std_mulcov: none
	// rate_mean_mulcov: none
	// -------------- compute prior density --------------------------------
	dismod_at::prior_density_struct<double> logden;
	logden = dismod_at::prior_density(
		var_info, var_vec,
		age_table, time_table, prior_table, s_info_vec
	);
	// --------------- check result ------------------------------------------
	// 2DO
	return ok;
}
// END C++
