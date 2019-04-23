/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/old2new_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin old2new_command$$
$spell
	dismod
	var
	mulcov
$$

$section Command That Converts an Old Database to New Format$$

$head Syntax$$
$codei%dismod_at %database% old2new%$$

$head Discussion$$
Currently, the only conversion is for the var table.

$head Var Table$$
This command adds the $cref/mulcov_id/var_table/mulcov_id/$$
column to the var table.
This column was added to the var table on
$cref/04-20/whats_new_2019/04-20/$$.

$end
*/

// ----------------------------------------------------------------------------
void old2new_command(
	sqlite3*                db          ,
	const db_input_struct&  db_input    ,
	const pack_info&        pack_object )
{	using std::string;
	using CppAD::to_string;
	using CppAD::vector;
	//
	string msg = "old2new command has nothing to change: ";
	//
	// check if var table exists
	string sql_cmd = "SELECT name FROM sqlite_master WHERE name='var'";
	char   sep = ',';
	string sql_ret = exec_sql_cmd(db, sql_cmd, sep);
	if( sql_ret.size() == 0 )
	{	msg += "No var table in database.";
		log_message(db, &std::cout, "warning", msg);
		return;
	}
	//
	// check number of rows in var table
	sql_cmd = "SELECT COUNT(*) FROM var";
	sql_ret = exec_sql_cmd(db, sql_cmd, sep);
	size_t len_var_table = std::atoi( sql_ret.c_str() );
	if( len_var_table != pack_object.size() )
	{	msg = "var table length does not agree with rest of database.";
		error_exit(msg);
	}
	//
	// check if mulcov_id column exists
	sql_cmd = "PRAGMA table_info(var)";
	sql_ret = exec_sql_cmd(db, sql_cmd, sep);
	size_t next_line = 0;
	while( next_line < sql_ret.size() )
	{	size_t start  = sql_ret.find(',', next_line) + 1;
		size_t stop   = sql_ret.find(',', start);
		string column = sql_ret.substr(start, stop - start);
		if( column == "mulcov_id" )
		{	msg += "Column mulcov_id already in var table.";
			log_message(db, &std::cout, "warning", msg);
			return;
		}
		next_line = sql_ret.find('\n', next_line) + 1;
	}
	//
	// add the mulcov_id column to var table
	sql_cmd = "ALTER TABLE var ADD COLUMN mulcov_id INTEGER";
	exec_sql_cmd(db, sql_cmd, sep);
	//
	// some table sizes
	size_t n_mulcov     = db_input.mulcov_table.size();
	size_t n_integrand  = db_input.integrand_table.size();
	size_t n_rate       = db_input.rate_table.size();
	//
	// covariate multiplers
	vector<size_t> count_meas_value(n_integrand);
	vector<size_t> count_meas_noise(n_integrand);
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	count_meas_value[integrand_id] = 0;
		count_meas_noise[integrand_id] = 0;
	}
	vector<size_t> count_rate_value(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
		count_rate_value[rate_id] = 0;
	//
	const vector<mulcov_struct>& mulcov_table( db_input.mulcov_table );
	for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
	if( mulcov_table[mulcov_id].smooth_id != DISMOD_AT_NULL_INT )
	{	mulcov_type_enum mulcov_type;
		mulcov_type         = mulcov_table[mulcov_id].mulcov_type;
		size_t rate_id      = mulcov_table[mulcov_id].rate_id;
		size_t integrand_id = mulcov_table[mulcov_id].integrand_id;
		//
		pack_info::subvec_info info;
		if( mulcov_type == rate_value_enum ) info =
			pack_object.mulcov_rate_value_info(
				rate_id, count_rate_value[rate_id]++
		);
		//
		else if( mulcov_type == meas_value_enum ) info =
			pack_object.mulcov_meas_value_info(
				integrand_id, count_meas_value[integrand_id]++
		);
		//
		else if( mulcov_type == meas_noise_enum ) info =
			pack_object.mulcov_meas_noise_info(
				integrand_id, count_meas_noise[integrand_id]++
		);
		else assert(false);
		//
		size_t offset = info.offset;
		size_t n_var  = info.n_var;
		for(size_t index = 0; index < n_var; index++)
		{	size_t var_id = offset + index;
			sql_cmd  = "UPDATE var SET mulcov_id = ";
			sql_cmd += CppAD::to_string(mulcov_id);
			sql_cmd += " WHERE var_id == ";
			sql_cmd += CppAD::to_string(var_id);
			exec_sql_cmd(db, sql_cmd, sep);
		}
	}
	return;
}

} // END_DISMOD_AT_NAMESPACE
