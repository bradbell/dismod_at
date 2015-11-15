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
$begin like_all_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ like_all: Example and Test$$

$code
$verbatim%example/devel/model/like_all_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/data_model.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool like_all_xam(void)
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

	// age and time smoohting grid indices
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
		dismod_at::smooth_info s_info(
			age_table, time_table, age_id_tmp, time_id,
			value_prior_id, dage_prior_id, dtime_prior_id,
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
	node_table[0].parent = DISMOD_AT_NULL_INT;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// covariate table
	size_t n_covariate = 0;
	vector<dismod_at::covariate_struct> covariate_table(n_covariate);
	//
	// data_table
	vector<dismod_at::data_struct> data_table(2);
	vector<double> data_cov_value(data_table.size() * n_covariate);
	//
	// parent node, time and age integrantion.
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
	}
	//
	// smooth_table
	size_t n_child        = 0;
	vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
	for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
	{	smooth_table[smooth_id].n_age  = s_info_vec[smooth_id].age_size();
		smooth_table[smooth_id].n_time = s_info_vec[smooth_id].time_size();
	}
	// mulcov_table
	vector<dismod_at::mulcov_struct> mulcov_table(0);
	// rate_table
	vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t smooth_id = 0;
		if( rate_id == dismod_at::pini_enum )
			smooth_id = 1; // only one age
		rate_table[rate_id].parent_smooth_id = smooth_id;
		rate_table[rate_id].child_smooth_id  = smooth_id;
	}
	// pack_object
	bool new_file = true;
	std::string file_name = "example.db";
	sqlite3* db = dismod_at::open_connection(file_name, new_file);
	dismod_at::pack_info pack_object(
		db, n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
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
	//
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
	data_object.replace_like(data_subset_obj);
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
				pack_vec[index] = age * time / (age_max*time_max);
			}
		}
	}
	// check results
	ok &= data_table.size() == data_subset_obj.size();
	bool hold_out = false;
	// parent node data does not depend on random effects
	bool random_depend = false;
	CppAD::vector< dismod_at::residual_struct<Float> > residual_vec =
		data_object.like_all(hold_out, random_depend, pack_vec);
	for(size_t data_id = 0; data_id < data_table.size(); data_id++)
	{	Float avg         = data_object.avg_no_ode(data_id, pack_vec);
		Float  wres       = residual_vec[data_id].wres;
		Float  loglike    = residual_vec[data_id].logden_smooth;
		loglike          -= fabs( residual_vec[data_id].logden_sub_abs );
		double delta      = data_table[data_id].meas_std;
		double y          = data_table[data_id].meas_value;
		double eta        = 1e-4;
		size_t density_id = data_table[data_id].density_id;
		bool log_density = density_id == dismod_at::log_gaussian_enum;
		log_density     |= density_id == dismod_at::log_laplace_enum;

		// check wres
		Float check;
		if( log_density )
			check = (log(y+eta) - log(avg+eta)) / log(1.0 + delta/(y+eta));
		else
			check = (y - avg) / delta;
		ok  &= fabs( 1.0 - wres / check ) <= eps;
		/*
		if( data_id == 0 )
			cout << "Debugging" << std::endl;
		cout << "wres = " << wres;
		cout << ", check = " << check;
		cout << ", relerr    = " << 1.0 - wres / check  << std::endl;
		*/
		// check loglike
		double pi =  3.14159265358979323846264338327950288;
		switch( density_id )
		{	double sigma;

			case dismod_at::uniform_enum:
			check = 0.0;
			break;

			case dismod_at::gaussian_enum:
			check = - log( delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
			break;

			case dismod_at::laplace_enum:
			check = - log( delta * sqrt(2.0) ) - sqrt(2.0) * fabs(wres);
			break;

			case dismod_at::log_gaussian_enum:
			sigma = log(1.0 + delta / (y + eta) );
			check = - log( sigma * sqrt(2.0 * pi) ) - wres * wres / 2.0;
			break;

			case dismod_at::log_laplace_enum:
			sigma = log(1.0 + delta / (y + eta) );
			check = - log( sigma * sqrt(2.0) ) - sqrt(2.0) * fabs(wres);
			break;

			default:
			assert(false);
		}
		if( density_id == dismod_at::uniform_enum )
			ok &= check == loglike;
		else
		{	Float relerr = 1.0 - loglike / check;
			ok &= fabs( relerr ) <= eps;
		}
		/*
		cout << "loglike = " << loglike;
		cout << ", check = " << check;
		if( density_id != dismod_at::uniform_enum )
		{	Float relerr = 1.0 - loglike / check;
			cout << ", relerr    = " << relerr;
			ok &= fabs( relerr ) <= eps;
		}
		cout << std::endl;
		*/
	}

	return ok;
}
// END C++
