/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/simulate_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/sim_random.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/meas_noise_effect.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin simulate_command$$

$section The Simulate Command$$
$spell
	sim
	avgint
	var
	dismod
	arg
	std
	covariates
$$

$head Syntax$$
$codei%dismod_at %database% simulate %number_simulate%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head number_simulate$$
Is the number of simulations. Each simulation contains a complete
data set.

$head meas_value$$
The $cref/meas_value/data_table/meas_value/$$ in the data
table is not used.

$head truth_var_table$$
The $cref truth_var_table$$ is an addition input table for this command.
It specifies the values for the
$cref model_variables$$ used during the simulation.
This table can be create by the
$cref/set_command/set_command/table_out/truth_var/$$,
or the user can create it directly with the aid of the
$cref var_table$$ (created by the $cref init_command$$).

$head data_sim_table$$
A new $cref data_sim_table$$ is created by this command.
It contains $icode number_simulate$$ values
for each $cref/data_id/data_subset_table/data_id/$$ in the data_subset table.
Hence the number of rows in $cref data_sim_table$$ is
$icode number_simulate$$ times the number of rows in $cref data_subset_table$$.

$head prior_sim_table$$
A new $cref prior_sim_table$$ is created by this command.
It contains $icode number_simulate$$ values
for each $cref/var_id/var_table/var_id/$$ in the var table.
Hence the number of rows in $cref data_sim_table$$ is
$icode number_simulate$$ times the number of rows in $cref var_table$$.


$children%example/get_started/simulate_command.py
%$$
$head Example$$
The file $cref simulate_command.py$$ contains an example and test
using this command.

$end
*/
void simulate_command(
	const std::string&                       number_simulate   ,
	const std::string&                       meas_noise_effect ,
	sqlite3*                                 db                ,
	const CppAD::vector<subset_data_struct>& subset_data_obj   ,
	data_model&                              data_object       ,
	const pack_prior&                        var2prior         ,
	const pack_info&                         pack_object       ,
	const db_input_struct&                   db_input          ,
	// effectively const
	std::map<std::string, std::string>&      option_map
)
{
	using std::string;
	using CppAD::vector;
	using CppAD::to_string;
	//
	const vector<prior_struct>&     prior_table( db_input.prior_table );
	const vector<density_enum>&     density_table( db_input.density_table );
	const vector<subgroup_struct>&  subgroup_table( db_input.subgroup_table );
	// -----------------------------------------------------------------------
	string msg;
	int tmp = std::atoi( number_simulate.c_str() );
	if( tmp <= 0 )
	{	msg  = "dismod_at simulate command number_simulate = ";
		msg += number_simulate + " is not an integer greater than zero";
		error_exit(msg);
	}
	size_t n_simulate = size_t(tmp);
	// -----------------------------------------------------------------------
	// read truth_var table into truth_var
	vector<double> truth_var;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	get_table_column(db, table_name, column_name, truth_var);
	// ----------------- data_sim_table ----------------------------------
	string sql_cmd = "drop table if exists data_sim";
	exec_sql_cmd(db, sql_cmd);
	//
	table_name      = "data_sim";
	size_t n_col    = 3;
	size_t n_subset = subset_data_obj.size();
	size_t n_row    = n_simulate * n_subset;
	vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]   = "simulate_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "data_subset_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "data_sim_value";
	col_type[2]   = "real";
	col_unique[2] = false;
	//
	// for each measurement in the data_subset table
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{
		//
		// compute the average integrand, avg
		double avg = data_object.average(subset_id, truth_var);
		//
		// density corresponding to this data point
		density_enum density = subset_data_obj[subset_id].density;
		//
		// data table information
		double eta          = subset_data_obj[subset_id].eta;
		double nu           = subset_data_obj[subset_id].nu;
		//
		// data table adjusted standard deviation, delta
		double delta;
		data_object.like_one(subset_id, truth_var, avg, delta);
		//
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	// for each simulate_index
			//
			// sim_value
			double sim_value   = sim_random(density, avg, delta, eta, nu);
			//
			size_t data_sim_id = sim_index * n_subset + subset_id;
			row_value[data_sim_id * n_col + 0] = to_string( sim_index );
			row_value[data_sim_id * n_col + 1] = to_string( subset_id );
			row_value[data_sim_id * n_col + 2] = to_string( sim_value );
		}
	}
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// ----------------- prior_sim_table ----------------------------------
	sql_cmd = "drop table if exists prior_sim";
	exec_sql_cmd(db, sql_cmd);
	//
	table_name    = "prior_sim";
	n_col         = 5;
	size_t n_var  = var2prior.size();
	n_row         = n_simulate * n_var;
	col_name.resize(n_col);
	col_type.resize(n_col);
	col_unique.resize(n_col);
	row_value.resize(n_col * n_row);
	//
	col_name[0]   = "simulate_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "var_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "prior_sim_value";
	col_name[3]   = "prior_sim_dage";
	col_name[4]   = "prior_sim_dtime";
	for(size_t k = 0; k < 3; k++)
	{	col_type[2+k]   = "real";
		col_unique[2+k] = false;
	}
	// -----------------------------------------------------------------------
	// simulate value for mean of prior for each variable in the var table
	vector<double> sim_prior_value(n_simulate * n_var);
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{	//
		// prior id for mean of this this variable
		size_t prior_id[3];
		vector<string> sim_str(3);
		prior_id[0]        = var2prior.value_prior_id(var_id);
		prior_id[1]        = var2prior.dage_prior_id(var_id);
		prior_id[2]        = var2prior.dtime_prior_id(var_id);
		double const_value = var2prior.const_value(var_id);
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	for(size_t k = 0; k < 3; ++k)
			if( k == 0 && ! std::isnan(const_value) )
			{	assert( prior_id[k] == DISMOD_AT_NULL_SIZE_T );
				sim_prior_value[sim_index * n_var + var_id] = const_value;
				sim_str[0] = to_string(const_value);
			}
			else if( prior_id[k] == DISMOD_AT_NULL_SIZE_T )
			{	assert( k != 0 );
				// The default prior is a uniform on [-inf, +inf]
				// cannot simulate from this distribution
				sim_str[k] = "null";
			}
			else
			{	double lower = prior_table[ prior_id[k] ].lower;
				double upper = prior_table[ prior_id[k] ].upper;
				double mean  = prior_table[ prior_id[k] ].mean;
				double std   = prior_table[ prior_id[k] ].std;
				double eta   = prior_table[ prior_id[k] ].eta;
				double nu    = prior_table[ prior_id[k] ].nu;
				//
				int density_id = prior_table[prior_id[k]].density_id;
				density_enum density = density_table[density_id];
				//
				if( density == uniform_enum )
					sim_str[k] = "null";
				else
				{	double sim = sim_random(density, mean, std, eta, nu);
					//
					sim = std::min(sim, upper);
					sim = std::max(sim, lower);
					//
					sim_str[k] = to_string( sim );
					//
					// store value prior for later use by zero sum constraints
					if( k == 0 )
						sim_prior_value[sim_index * n_var + var_id] = sim;
				}
			}
			//
			size_t prior_sim_id = sim_index * n_var + var_id;
			row_value[prior_sim_id * n_col + 0] = to_string( sim_index );
			row_value[prior_sim_id * n_col + 1] = to_string( var_id );
			row_value[prior_sim_id * n_col + 2] = sim_str[0];
			row_value[prior_sim_id * n_col + 3] = sim_str[1];
			row_value[prior_sim_id * n_col + 4] = sim_str[2];
		}
	}
	// ----------------------------------------------------------------------
	// Enforce zero_sum_child_rate contraints
	//
	// n_child
	size_t n_child = pack_object.child_size();
	//
	// zero_sum_child_rate
	size_t n_rate      = size_t(number_rate_enum);
	size_t option_size = option_map["zero_sum_child_rate"].size();
	vector<bool> zero_sum_child_rate(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	string rate_name = get_rate_name(rate_id);
		size_t found     = option_map["zero_sum_child_rate"].find( rate_name );
		zero_sum_child_rate[rate_id] = found < option_size;
	}
	// for each simulation index and each rate
	for(size_t sim_index = 0; sim_index < n_simulate; ++sim_index)
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	if( zero_sum_child_rate[rate_id] )
	{	// packing information for first child
		pack_info::subvec_info
			info_0 = pack_object.node_rate_value_info(rate_id, 0);
		//
		// child smoothing id for first rate
		size_t smooth_id = info_0.smooth_id;
		if( smooth_id != DISMOD_AT_NULL_SIZE_T && n_child > 0)
		{	//
			// number of grid points in child smoothing for this rate
			size_t n_grid = info_0.n_var;
			//
			// var_id corresponding to each child
			vector<size_t> var_id_tmp(n_child);
			//
			// loop over grid points
			for(size_t k = 0; k < n_grid; k++)
			{	// initialize summation for this grid point
				double sum_random = 0.0;
				//
				// sum over childern
				for(size_t j = 0; j < n_child; j++)
				{	// packing information for this child
					pack_info::subvec_info
						info_j = pack_object.node_rate_value_info(rate_id, j);
					//
					// all the children hav the same smoothing
					assert( info_j.smooth_id == info_0.smooth_id );
					assert( info_j.n_var     == info_0.n_var );
					//
					// offset for this child
					size_t offset = info_j.offset;
					//
					// variable index for this grid point and child
					size_t var_id = offset + k;
					var_id_tmp[j] = var_id;
					//
					// add to sum of random effect for this grid point
					sum_random += sim_prior_value[sim_index * n_var + var_id];
				}
				//
				// zero mean this grid point
				for(size_t j = 0; j < n_child; ++j)
				{	size_t var_id = var_id_tmp[j];
					double value = sim_prior_value[sim_index * n_var + var_id];
					value       -= sum_random / double(n_child);
					size_t prior_sim_id = sim_index * n_var + var_id;
					//
					// overwrite the value prior to be zero mean
					row_value[prior_sim_id * n_col + 2] = to_string(value);
				}
			}
		}
	}
	// ----------------------------------------------------------------------
	// Enforce zero_sum_mulcov_group contraints
	//
	// n_group
	size_t n_group = pack_object.group_size();
	//
	// zero_sum_mulcov_group
	option_size    = option_map["zero_sum_mulcov_group"].size();
	vector<bool> zero_sum_mulcov_group(n_group);
	for(size_t group_id = 0; group_id < n_group; group_id++)
	{	size_t first_subgroup_id = pack_object.first_subgroup_id(group_id);
		string group_name = subgroup_table[first_subgroup_id].group_name;
		size_t found = option_map["zero_sum_mulcov_group"].find(group_name);
		zero_sum_mulcov_group[group_id] = found < option_size;
	}
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_id[2];
	n_id[0] = n_rate;
	n_id[1] = n_integrand;
	// for each simulation index and each rate
	for(size_t sim_index = 0; sim_index < n_simulate; ++sim_index)
	for(size_t loop = 0; loop < 2; ++loop)
	for(size_t id = 0; id < n_id[loop]; ++id)
	{	size_t rate_id      = DISMOD_AT_NULL_SIZE_T;
		size_t integrand_id = DISMOD_AT_NULL_SIZE_T;
		if( loop == 0 )
			rate_id = id;
		else
			integrand_id = id;
		//
		// number of covariate multipliers for this rate or integrand
		size_t n_cov;
		if( loop == 0 )
			n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
		else
			n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
		//
		// for each covariate multiplier
		for(size_t j = 0; j < n_cov; ++j)
		{	// number of subgroups and packing information for first subgroup
			size_t n_sub;
			pack_info::subvec_info info_0;
			if( loop == 0 )
			{	n_sub  = pack_object.subgroup_rate_value_n_sub(rate_id, j);
				info_0 = pack_object.subgroup_rate_value_info(rate_id, j, 0);
			}
			else
			{	n_sub  = pack_object.subgroup_meas_value_n_sub(integrand_id, j);
				info_0 = pack_object.subgroup_meas_value_info(
					integrand_id, j, 0
				);
			}
			//
			// only consider groups that are zero sum contrained
			if( zero_sum_mulcov_group[info_0.group_id] )
			{	// smoothing for subgroups
				assert( info_0.smooth_id != DISMOD_AT_NULL_SIZE_T );
				//
				// number of grid points in subgroup smoothing for this mulcov
				size_t n_grid = info_0.n_var;
				//
				// var_id corresponding to each subgroup
				vector<size_t> var_id_tmp(n_sub);
				//
				// loop over grid points
				for(size_t ell = 0; ell < n_grid; ++ell)
				{	// initialize sum for this grid point
					double sum_random = 0.0;
					//
					// sum over subgroups
					for(size_t k = 0; k < n_sub; ++k)
					{	// packing information for this subgroup
						pack_info::subvec_info info_k;
						if( loop == 0 )
							info_k = pack_object.subgroup_rate_value_info(
								rate_id, j, k
							);
						else
							info_k = pack_object.subgroup_meas_value_info(
								integrand_id, j, k
							);
						//
						// all subgroups have the same smoothing
						assert( info_k.smooth_id == info_0.smooth_id );
						assert( info_k.n_var     == info_0.n_var );
						//
						// offset for this subgroup
						size_t offset = info_k.offset;
						//
						// var_id for this grid point and subgroup
						size_t var_id = offset + ell;
						var_id_tmp[k] = var_id;
						//
						// add to sum of random effect for this grid point
						sum_random +=
							sim_prior_value[sim_index * n_var + var_id];
					}
					//
					// zero mean this grid point's sum
					for(size_t k = 0; k < n_sub; ++k)
					{	size_t var_id = var_id_tmp[j];
						double value =
							sim_prior_value[sim_index * n_var + var_id];
						value       -= sum_random / double(n_sub);
						size_t prior_sim_id = sim_index * n_var + var_id;
						//
						// overwrite the value prior to be zero mean
						row_value[prior_sim_id * n_col + 2] = to_string(value);
					}
				}
			}
		}
	}
	// ------------------------------------------------------------------------
	// create prior_sim table
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}

} // END_DISMOD_AT_NAMESPACE
