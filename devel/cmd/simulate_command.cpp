/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
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
	const std::string&                                  number_simulate ,
	sqlite3*                                            db              ,
	const CppAD::vector<dismod_at::integrand_struct>&   integrand_table ,
	const CppAD::vector<dismod_at::data_subset_struct>& data_subset_obj ,
	dismod_at::data_model&                              data_object     ,
	const dismod_at::pack_prior&                        var2prior       ,
	const CppAD::vector<dismod_at::prior_struct>&       prior_table     ,
	const CppAD::vector<dismod_at::density_enum>&       density_table
)
{
	using std::string;
	using CppAD::vector;
	using CppAD::to_string;
	// -----------------------------------------------------------------------
	string msg;
	int tmp = std::atoi( number_simulate.c_str() );
	if( tmp <= 0 )
	{	msg  = "dismod_at simulate command number_simulate = ";
		msg += number_simulate + " is not an integer greater than zero";
		dismod_at::error_exit(msg);
	}
	size_t n_simulate = size_t(tmp);
	// -----------------------------------------------------------------------
	// read truth_var table into truth_var
	vector<double> truth_var;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	dismod_at::get_table_column(db, table_name, column_name, truth_var);
	// ----------------- data_sim_table ----------------------------------
	string sql_cmd = "drop table if exists data_sim";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name      = "data_sim";
	size_t n_col    = 4;
	size_t n_subset = data_subset_obj.size();
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
	col_name[3]   = "data_sim_delta";
	col_type[3]   = "real";
	col_unique[3] = false;
	//
	// for each measurement in the data_subset table
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	//
		// compute the average integrand, avg
		double avg = data_object.average(subset_id, truth_var);
		//
		// compute the adjusted standard deviation corresponding
		// to the values in the data table, delta.
		double sim_delta;
		data_object.like_one(subset_id, truth_var, avg, sim_delta);
		//
		// density corresponding to this data point
		dismod_at::density_enum density = data_subset_obj[subset_id].density;
		//
		double difference   = false;
		double eta          = data_subset_obj[subset_id].eta;
		double nu           = data_subset_obj[subset_id].nu;
		//
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	// for each simulate_index
			//
			// compute the simulated measurement value
			double sim_value   = dismod_at::sim_random(
				difference, density, avg, sim_delta, eta, nu
			);
			if( density == dismod_at::log_gaussian_enum
			||  density == dismod_at::log_laplace_enum
			||  density == dismod_at::log_students_enum )
				sim_value = std::max(sim_value, 0.0);
			//
			size_t data_sim_id = sim_index * n_subset + subset_id;
			row_value[data_sim_id * n_col + 0] = to_string( sim_index );
			row_value[data_sim_id * n_col + 1] = to_string( subset_id );
			row_value[data_sim_id * n_col + 2] = to_string( sim_value );
			row_value[data_sim_id * n_col + 3] = to_string( sim_delta );
		}
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// ----------------- prior_sim_table ----------------------------------
	sql_cmd = "drop table if exists prior_sim";
	dismod_at::exec_sql_cmd(db, sql_cmd);
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
	// for each measurement in the data_subset table
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{	//
		// prior id for mean of this this variable
		size_t prior_id[3];
		vector<string> sim_str(3);
		prior_id[0] = var2prior.value_prior_id(var_id);
		prior_id[1] = var2prior.dage_prior_id(var_id);
		prior_id[2] = var2prior.dtime_prior_id(var_id);
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	for(size_t k = 0; k < 3; ++k)
			if( prior_id[k] == DISMOD_AT_NULL_SIZE_T )
				sim_str[k] = "null";
			else
			{	double lower = prior_table[ prior_id[k] ].lower;
				double upper = prior_table[ prior_id[k] ].upper;
				double mean  = prior_table[ prior_id[k] ].mean;
				double std   = prior_table[ prior_id[k] ].std;
				double eta   = prior_table[ prior_id[k] ].eta;
				double nu    = prior_table[ prior_id[k] ].nu;
				//
				// k = 0 is a value prior, others are difference priors
				bool difference = k > 0;
				//
				int density_id = prior_table[prior_id[k]].density_id;
				dismod_at::density_enum density = density_table[density_id];
				//
				if( density == dismod_at::uniform_enum )
					sim_str[k] = "null";
				else
				{	double sim = dismod_at::sim_random(
						difference, density, mean, std, eta, nu
					);
					//
					sim = std::min(sim, upper);
					sim = std::max(sim, lower);
					//
					sim_str[k] = to_string( sim );
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
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}

} // END_DISMOD_AT_NAMESPACE
