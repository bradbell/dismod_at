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
$begin avg_yes_ode_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ avg_yes_ode: Example and Test$$
$index example, C++ avg_yes_ode$$
$index avg_yes_ode, C++ example$$

$code
$verbatim%example/devel/model/avg_yes_ode_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/data_model.hpp>

bool avg_yes_ode_xam(void)
{	bool   ok = true;
	size_t i, k;
	using CppAD::abs;
	using CppAD::vector;
	using std::cout;
	typedef CppAD::AD<double> Float;
	//
	// ode_step_size
	double ode_step_size = 3.0;
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
	dismod_at::weight_info w_info(age_id, time_id, weight);
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
			age_id_tmp, time_id, value_prior_id, dage_prior_id, dtime_prior_id,
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
	// node_table:
	CppAD::vector<dismod_at::node_struct> node_table(3);
	node_table[0].parent = -1; // node zero has two children
	node_table[1].parent = 0;
	node_table[2].parent = 0;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// data_table
	vector<dismod_at::data_struct> data_table(1);
	size_t data_id = 0;
	vector<double> x(0);
	data_table[data_id].integrand_id = dismod_at::prevalence_enum;
	data_table[data_id].node_id      = 1; // child node
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = 0.0;
	data_table[data_id].age_upper    = 100.0;
	data_table[data_id].time_lower   = 1990.0;
	data_table[data_id].time_upper   = 2000.0;
	data_table[data_id].meas_value   = 0.0;
	data_table[data_id].meas_std     = 1e-3;
	data_table[data_id].density_id   = dismod_at::uniform_enum;
	data_table[data_id].x            = x;
	//
	// smooth_table
	size_t n_child        = 2;
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
		rate_table[rate_id].child_smooth_id = smooth_id;
	}
	// pack_object
	dismod_at::pack_info pack_object(
		n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	//
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
	//
	// pack_vec
	double beta_parent   = 0.01;
	double random_effect = log(2.0);
	double beta          = beta_parent * exp( random_effect );
	vector<Float> pack_vec( pack_object.size() );
	dismod_at::pack_info::subvec_info info;
	size_t n_rate = dismod_at::number_rate_enum;
	for(size_t child_id = 0; child_id <= n_child; child_id++)
	{	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
		{	info = pack_object.rate_info(rate_id, child_id);
			for(k = 0; k < info.n_var; k++)
			{	if( rate_id == size_t(dismod_at::iota_enum) )
				{	if( child_id == n_child )
						pack_vec[info.offset + k] = beta_parent;
					else
						pack_vec[info.offset + k] = random_effect;
				}
				else
					pack_vec[info.offset + k] = 0.00;
			}
		}
	}
	/*
	Solution is S(a) = exp( -beta * a ), P(a) = C(a) = 1.0 - exp( -beta * a )
	int_b^c P(a) / (c - b) = [ a  + exp( -beta a ) / beta ]_b^c / (c - b)
	*/
	using CppAD::exp;
	data_id = 0;
	Float avg      = data_object.avg_yes_ode(data_id, pack_vec);
	double b       = data_table[data_id].age_lower;
	double c       = data_table[data_id].age_upper;
	double check   = c - b + ( exp(-beta * c) - exp(-beta * b) ) / beta;
	check         /= (c - b);
	ok             &= fabs( 1.0 - avg / check ) <= 1e-3;
	/*
	cout << "Debugging" << std::endl;
	cout << "avg = " << avg;
	cout << ", check = " << check;
	cout << ", relerr    = " << 1.0 - avg / check  << std::endl;
	*/
	return ok;
}
// END C++
