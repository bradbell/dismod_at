// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
Test computing data model values on a subset of data table.
*/
# include <limits>
# include <dismod_at/data_model.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

namespace {
	double check_avg(const dismod_at::data_struct& data_row)
	{	double a0 = data_row.age_lower;
		double a1 = data_row.age_upper;
		double t0 = data_row.time_lower;
		double t1 = data_row.time_upper;
		// integral of age * time from a0 to a1 and t0 to t1
		// divided by (a1 - a0) time (t1 - t0) is
		return (a0 + a1) * (t0 + t1) / 4.0;
	}
}

bool data_model_subset(void)
{	bool   ok = true;
	size_t i, j, k;
	using CppAD::abs;
	using CppAD::vector;
	using std::cout;
	typedef CppAD::AD<double> Float;
	Float eps = CppAD::numeric_limits<Float>::epsilon() * 100;
	//
	// ode_step_size
	double ode_step_size = 30.0;
	//
	// age_table
	// (make sure that ode grid lands on last age table point)
	double age = 0.0;
	vector<double> age_table;
	age_table.push_back(age);
	while( age < 100. )
	{	age += ode_step_size;
		age_table.push_back(age);
	}
	size_t n_age_table = age_table.size();
	double age_min     = age_table[0];
	double age_max     = age_table[n_age_table - 1];
	//
	// time_table
	// (make sure that ode grid lands on last time table point)
	double time = 1980.0;
	vector<double> time_table;
	time_table.push_back(time);
	while( time < 2020.0 )
	{	time += ode_step_size;
		time_table.push_back(time);
	}
	size_t n_time_table = time_table.size();
	double time_min     = time_table[0];
	double time_max     = time_table[n_time_table - 1];

	// age and time smoothing grid indices
	size_t n_age_si   = 3;
	size_t n_time_si  = 2;
	vector<size_t> age_id(n_age_si), time_id(n_time_si);
	age_id[0]   = 0;
	age_id[1]   = n_age_table / 2;
	age_id[2]   = n_age_table - 1;
	time_id[0]  = 0;
	time_id[1]  = n_time_table - 1;
	//
	// w_info_vec
	// weight value should not matter when constant
	size_t n_si = n_age_si * n_time_si;
	vector<double> weight(n_si);
	for(k = 0; k < n_si; k++)
		weight[k] = 0.5;
	dismod_at::weight_info w_info(
		age_table, time_table, age_id, time_id, weight
	);
	vector<dismod_at::weight_info> w_info_vec(1);
	w_info_vec[0] = w_info;
	//
	// s_info_vec
	vector<dismod_at::smooth_info> s_info_vec(2);
	size_t mulstd_value = 1, mulstd_dage = 1, mulstd_dtime = 1;
	for(size_t smooth_id = 0; smooth_id < 2; smooth_id++)
	{	vector<size_t> age_id_tmp;
		if( smooth_id == 0 )
		{	n_si       = n_age_si * n_time_si;
			age_id_tmp = age_id;
		}
		else
		{	n_si = n_time_si;
			age_id_tmp.resize(1);
			age_id_tmp[0] = 0;
		}
		//
		vector<size_t> value_prior_id(n_si),
			dage_prior_id(n_si), dtime_prior_id(n_si);
		vector<double> const_value(n_si);
		dismod_at::smooth_info s_info(
			age_table, time_table, age_id_tmp, time_id,
			value_prior_id, dage_prior_id, dtime_prior_id, const_value,
			mulstd_value, mulstd_dage, mulstd_dtime
		);
		s_info_vec[smooth_id] = s_info;
	}
	//
	// integrand_table
	size_t n_integrand = dismod_at::number_integrand_enum;
	vector<dismod_at::integrand_struct> integrand_table(n_integrand);
	for(i = 0; i < n_integrand; i++)
	{	integrand_table[i].integrand = dismod_at::integrand_enum(i);
		integrand_table[i].eta       = 1e-6;
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
	// node_table:    0
	//              1    2
	//                  3  4
	size_t n_node = 5;
	CppAD::vector<dismod_at::node_struct> node_table(n_node);
	node_table[0].parent = DISMOD_AT_NULL_INT;
	node_table[1].parent =  0;
	node_table[2].parent =  0;
	node_table[3].parent =  2;
	node_table[4].parent =  2;
	//
	// choose parent note so that only a subset of the data is used
	size_t parent_node_id = 2;
	//
	// covariate table
	size_t n_covariate = 0;
	vector<dismod_at::covariate_struct> covariate_table(n_covariate);
	//
	// data_table
	size_t n_data = n_node;
	vector<dismod_at::data_struct> data_table(n_data);
	vector<double> data_cov_value(n_data * n_covariate);
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	// just evaluating integrand at a point
		data_table[data_id].integrand_id = dismod_at::mtother_enum;
		data_table[data_id].node_id      = data_id;
		data_table[data_id].weight_id    = 0;
		data_table[data_id].age_lower    = 35.0;
		data_table[data_id].age_upper    = 35.0;
		data_table[data_id].time_lower   = 1990.0 + data_id * 5;
		data_table[data_id].time_upper   = 1990.0 + data_id * 5;
		data_table[data_id].meas_value   = 0.0;
		data_table[data_id].meas_std     = 1e-3;
		data_table[data_id].density_id   = dismod_at::uniform_enum;
	}
	//
	// smooth_table
	size_t n_child        = 2;
	vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	smooth_table[smooth_id].n_age  = s_info_vec[smooth_id].age_size();
		smooth_table[smooth_id].n_time = s_info_vec[smooth_id].time_size();
	}
	// mul_cov
	vector<dismod_at::mulcov_struct> mulcov_table(0);
	// rate_table
	vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t smooth_id = 0;
		if( rate_id == dismod_at::pini_enum )
			smooth_id = 1; // only one age
		rate_table[rate_id].parent_smooth_id = smooth_id;
		rate_table[rate_id].child_smooth_id = smooth_id;
	}
	//
	// pack_object
	std::string file_name = "example.db";
	dismod_at::pack_info pack_object(
		n_integrand, n_child, smooth_table, mulcov_table, rate_table
	);
	//
	// child_info
	dismod_at::child_info child_object(
		parent_node_id ,
		node_table     ,
		data_table
	);
	// data_subset
	vector<dismod_at::data_subset_struct> data_subset_obj;
	vector<double> data_subset_cov_value;
	data_subset(
		data_table,
		data_cov_value,
		covariate_table,
		child_object,
		data_subset_obj,
		data_subset_cov_value
	);
	// data_model
	dismod_at::data_model data_object(
		parent_node_id,
		n_covariate,
		n_age_ode,
		n_time_ode,
		ode_step_size,
		age_table,
		time_table,
		integrand_table,
		node_table,
		data_subset_obj,
		data_subset_cov_value,
		w_info_vec,
		s_info_vec,
		pack_object,
		child_object
	);
	//
	// pack_vec
	vector<Float> pack_vec( pack_object.size() );
	dismod_at::pack_info::subvec_info info;
	for(size_t child_id = 0; child_id <= n_child; child_id++)
	{	info = pack_object.rate_info(dismod_at::omega_enum, child_id);
		dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
		for(i = 0; i < s_info.age_size(); i++)
		{	double age = age_table[ s_info.age_id(i) ];
			for(j = 0; j < s_info.time_size(); j++)
			{	double time    = time_table[ s_info.time_id(j) ];
				size_t index   = info.offset + i * s_info.time_size() + j;
				if( child_id == n_child )
					pack_vec[index] = age * time / (age_max*time_max);
				else
					pack_vec[index] = 0.0;
			}
		}
	}
	// check results
	size_t n_sample = data_subset_obj.size();
	ok &= n_sample == 3;
	for(size_t subset_id = 0; subset_id < n_sample; subset_id++)
	{	Float avg      = data_object.avg_no_ode(subset_id, pack_vec);
		size_t data_id = data_subset_obj[subset_id].original_id;
		double check   = check_avg(data_table[data_id]) / (age_max*time_max);
		ok            &= fabs( 1.0 - avg / check ) <= eps;
	}

	return ok;
}
