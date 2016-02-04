// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin pack_diff_prior_xam.cpp$$
$spell
	xam
	diff
$$

$section C++ pack_diff_prior: Example and Test$$

$code
$srcfile%example/devel/utility/pack_diff_prior_xam.cpp%0
	%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/open_connection.hpp>

# define DISMOD_AT_PRIOR_DENSITY_XAM_TRACE 0

bool pack_diff_prior_xam(void)
{	bool   ok = true;
	size_t i, j;
	using CppAD::vector;
	using std::cout;
	using std::endl;
	double inf = std::numeric_limits<double>::infinity();


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
	//
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
	mulstd_value = 0;
	mulstd_dage  = 1;
	mulstd_dtime = 2;
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
		age_table, time_table, age_id, time_id,
		value_prior_id, dage_prior_id, dtime_prior_id,
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
	mulstd_value = 0;
	mulstd_dage  = 1;
	mulstd_dtime = 2;
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
		age_table, time_table, age_id, time_id,
		value_prior_id, dage_prior_id, dtime_prior_id,
		mulstd_value, mulstd_dage, mulstd_dtime
	);
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
	size_t n_integrand = 0;
	size_t n_child     = 1;
	bool new_file = true;
	std::string file_name = "example.db";
	sqlite3* db = dismod_at::open_connection(file_name, new_file);
	dismod_at::pack_info pack_object(
		db, n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	// ----------------------- diff_prior -------------------------------
	vector<dismod_at::diff_prior_struct> diff_prior =
		dismod_at::pack_diff_prior(pack_object, s_info_vec);
	size_t n_diff_prior = diff_prior.size();
	//
	dismod_at::pack_info::subvec_info info;
	//
	// check rates
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	info = pack_object.rate_info(rate_id, child_id);
			dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
			n_age  = s_info.age_size();
			n_time = s_info.time_size();
			if( n_age > 1 )
			{	for(i = 0; i < n_age-1; i++)
				{	for(j = 0; j < n_time; j++)
					{	size_t minus_var_id = info.offset + i * n_time + j;
						size_t plus_var_id  = info.offset + (i+1) * n_time + j;
						size_t prior_id     = s_info.dage_prior_id(i, j);
						size_t count = 0;
						for(size_t k = 0; k < n_diff_prior; k++)
						{	bool match = true;
							match &= diff_prior[k].minus_var_id==minus_var_id;
							match &= diff_prior[k].plus_var_id == plus_var_id;
							if( match )
							{	ok &= diff_prior[k].prior_id == prior_id;
								count++;
							}
						}
						ok &= count == 1;
					}
				}
			}
			if( n_time > 1 )
			{	for(i = 0; i < n_age; i++)
				{	for(j = 0; j < n_time-1; j++)
					{	size_t minus_var_id = info.offset + i * n_time + j;
						size_t plus_var_id  = info.offset + i * n_time + j + 1;
						size_t prior_id     = s_info.dtime_prior_id(i, j);
						size_t count = 0;
						for(size_t k = 0; k < n_diff_prior; k++)
						{	bool match = true;
							match &= diff_prior[k].minus_var_id==minus_var_id;
							match &= diff_prior[k].plus_var_id == plus_var_id;
							if( match )
							{	ok &= diff_prior[k].prior_id == prior_id;
								count++;
							}
						}
						ok &= count == 1;
					}
				}
			}
		}
	}
	return ok;
}
// END C++
