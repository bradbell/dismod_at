// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <limits>
# include <dismod_at/data_model.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/age_avg_grid.hpp>

// Testing measurement covariate multipliers

namespace {
	double age_max_, time_max_, x_j = 0.5;
	double mulcov(double age, double time)
	{	double a = age / age_max_;
		double t = time / time_max_;
		double r = 0.1 + a + t + a * t;
		return r;
	}
	double avg_mulcov(
		double a0 ,
		double a1 ,
		double t0 ,
		double t1 )
	{	double sum = 0;
		sum += x_j * mulcov(a0, t0);
		sum += x_j * mulcov(a0, t1);
		sum += x_j * mulcov(a1, t0);
		sum += x_j * mulcov(a1, t1);
		return sum / 4.0;
	}
}
bool meas_mulcov(void)
{	bool   ok = true;
	using CppAD::vector;
	using std::cout;
	typedef CppAD::AD<double> Float;
	Float eps = CppAD::numeric_limits<Float>::epsilon() * 100;
	//
	// ode_step_size
	double ode_step_size = 10.0;
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
	age_max_           = age_table[n_age_table - 1];
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
	time_max_           = time_table[n_time_table - 1];
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
	for(size_t k = 0; k < n_si; k++)
		weight[k] = 0.5;
	dismod_at::weight_info w_info(
		age_table, time_table, age_id, time_id, weight
	);
	vector<dismod_at::weight_info> w_info_vec(2);
	w_info_vec[0] = w_info;
	//
	// prior table
	double inf = std::numeric_limits<double>::infinity();
	double nan = std::numeric_limits<double>::quiet_NaN();
	vector<dismod_at::prior_struct> prior_table(1);
	prior_table[0].prior_name = "prior_zero";
	prior_table[0].density_id = 0;
	prior_table[0].lower      = -inf;
	prior_table[0].upper      = +inf;
	prior_table[0].mean       = 0.0;
	prior_table[0].std        = nan;
	prior_table[0].eta        = nan;
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
		for(size_t i = 0; i < n_si; i++)
			value_prior_id[i] = 0;
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
	for(size_t i = 0; i < n_integrand; i++)
	{	integrand_table[i].integrand       = dismod_at::integrand_enum(i);
		integrand_table[i].minimum_meas_cv = 0.0;
	}
	//
	// node_table:    0
	//              1    2
	//                  3  4
	CppAD::vector<dismod_at::node_struct> node_table(5);
	node_table[0].parent = DISMOD_AT_NULL_INT;
	node_table[1].parent =  0;
	node_table[2].parent =  0;
	node_table[3].parent =  2;
	node_table[4].parent =  2;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// covariate table
	size_t n_covariate = 1;
	vector<dismod_at::covariate_struct> covariate_table(n_covariate);
	covariate_table[0].covariate_name  = "sex";
	covariate_table[0].reference       = 0.0;
	covariate_table[0].max_difference  = 0.6;
	//
	// data_table
	vector<dismod_at::data_struct> data_table(1);
	vector<double> data_cov_value(data_table.size() * n_covariate);
	//
	double age_lower  = age_min + ode_step_size;
	double age_upper  = age_lower + 2.0 * ode_step_size;
	double time_lower = time_min;
	double time_upper = time_lower + ode_step_size;
	size_t data_id = 0;
	data_table[data_id].integrand_id =  int( dismod_at::mtother_enum );
	data_table[data_id].node_id      =  int( 0 );
	data_table[data_id].weight_id    =  int( 0 );
	data_table[data_id].age_lower    = age_lower;
	data_table[data_id].age_upper    = age_upper;
	data_table[data_id].time_lower   = time_lower;
	data_table[data_id].time_upper   = time_upper;
	data_table[data_id].meas_value   = 1e-2;
	data_table[data_id].meas_std     = 1e-3;
	data_table[data_id].eta          = 1e-6;
	data_table[data_id].density_id   =  int( dismod_at::gaussian_enum );
	//
	data_cov_value[0] = x_j;
	//
	// smooth_table
	vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	smooth_table[smooth_id].n_age  =
            int( s_info_vec[smooth_id].age_size() );
		smooth_table[smooth_id].n_time =
            int( s_info_vec[smooth_id].time_size() );
	}
	// mul_cov
	size_t omega_rate_id = dismod_at::omega_enum;
	vector<dismod_at::mulcov_struct> mulcov_table(2);
	mulcov_table[0].mulcov_type = dismod_at::meas_value_enum;
	mulcov_table[1].mulcov_type = dismod_at::meas_noise_enum;
	for(size_t mulcov_id = 0; mulcov_id < 2; mulcov_id++)
	{	mulcov_table[mulcov_id].rate_id      =  int( DISMOD_AT_NULL_INT );
		mulcov_table[mulcov_id].integrand_id =  int( dismod_at::mtother_enum );
		mulcov_table[mulcov_id].covariate_id =  int( 0 );
		mulcov_table[mulcov_id].smooth_id    =  int( 0 );
	}
	// rate_table
	vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t smooth_id = 0;
		if( rate_id == dismod_at::pini_enum )
			smooth_id = 1; // only one age
		rate_table[rate_id].parent_smooth_id =  int( smooth_id );
		rate_table[rate_id].child_smooth_id  =  int( smooth_id );
		rate_table[rate_id].child_nslist_id  =  int( DISMOD_AT_NULL_INT );
	}
	// child_info
	dismod_at::child_info child_object(
		parent_node_id ,
		node_table     ,
		data_table
	);
	size_t n_child = child_object.child_size();
	assert( n_child == 2 );
	// pack_object
	// values in child_id2node_id do not matter because child_nslist_id is null
	vector<size_t> child_id2node_id(n_child);
	vector<dismod_at::nslist_pair_struct> nslist_pair(0);
	dismod_at::pack_info pack_object(n_integrand,
		child_id2node_id, smooth_table, mulcov_table, rate_table, nslist_pair
	);
	// data_subset
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
	//
	// data_model
	double bound_random = std::numeric_limits<double>::infinity();
	bool        fit_simulated_data = false;
	std::string meas_noise_effect = "add_std_scale_all";
	std::string rate_case       = "iota_pos_rho_pos";
	std::string age_avg_split   = "";
	vector<double> age_avg_grid = dismod_at::age_avg_grid(
		ode_step_size, age_avg_split, age_table
	);
	dismod_at::data_model data_object(
		fit_simulated_data,
		meas_noise_effect,
		rate_case,
		bound_random,
		n_covariate,
		ode_step_size,
		age_avg_grid,
		age_table,
		time_table,
		integrand_table,
		mulcov_table,
		prior_table,
		data_subset_obj,
		data_subset_cov_value,
		w_info_vec,
		s_info_vec,
		pack_object,
		child_object
	);
	data_object.replace_like(data_subset_obj);
	//
	// pack_vec
	vector<Float> pack_vec( pack_object.size() );
	dismod_at::pack_info::subvec_info info;
	for(size_t child_id = 0; child_id <= n_child; child_id++)
	{	info = pack_object.rate_info(omega_rate_id, child_id);
		dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
		for(size_t i = 0; i < s_info.age_size(); i++)
		{	for(size_t j = 0; j < s_info.time_size(); j++)
			{	size_t index   = info.offset + i * s_info.time_size() + j;
				pack_vec[index] = 1.0;
			}
		}
	}
	size_t mtother_id = dismod_at::mtother_enum;
	dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
	for(size_t k = 0; k < 2; k++)
	{	if( k == 0 )
			info = pack_object.mulcov_meas_value_info(mtother_id, 0);
		else
			info = pack_object.mulcov_meas_noise_info(mtother_id, 0);
		for(size_t i = 0; i < s_info.age_size(); i++)
		{	age = age_table[ s_info.age_id(i) ];
			for(size_t j = 0; j < s_info.time_size(); j++)
			{	time           = time_table[ s_info.time_id(j) ];
				size_t index   = info.offset + i * s_info.time_size() + j;
				pack_vec[index] = mulcov(age, time);
			}
		}
	}

	// evaluate residual
	data_id = 0;
	Float avg_integrand = data_object.average(data_id, pack_vec);
	Float delta_out;
	dismod_at::residual_struct<Float> residual    =
		data_object.like_one(data_id, pack_vec, avg_integrand, delta_out);
	Float wres = residual.wres;
	//
	// average std mulcov
	double avg_mulcov_1 = avg_mulcov(
		age_lower, age_lower + ode_step_size,
		time_lower, time_lower + ode_step_size
	);
	double avg_mulcov_2  = avg_mulcov(
		age_lower + ode_step_size, age_lower + 2.0 *ode_step_size,
		time_lower,                time_lower + ode_step_size
	);
	double avg_noise_mulcov = (avg_mulcov_1 + avg_mulcov_2) / 2.0;
	//
	// check residual
	double y     = data_table[data_id].meas_value;
	double Delta = data_table[data_id].meas_std;
	double delta = Delta * (1.0 + avg_noise_mulcov);
	Float  check = (y - avg_integrand) / delta;
	ok          &= fabs( 1.0 - wres / check ) <= eps;
	//
	// check delta_out
	ok          &= fabs( 1.0 - delta_out / delta ) <= eps;
	/*
	if( data_id == 0 )
		cout << "Debugging" << std::endl;
	cout << "wres = " << wres;
	cout << ", check = " << check;
	cout << ", relerr    = " << 1.0 - wres / check  << std::endl;
	*/
	return ok;
}
