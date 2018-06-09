// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/set_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_table_column.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/create_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin set_command$$
$spell
	sim
	var
	dismod
$$

$section Directly Setting Table Values$$

$head Syntax$$
$codei%dismod_at %database% set option %name% %value%
%$$
$codei%dismod_at %database% set %table_out% %source%
%$$
$codei%dismod_at %database% set %table_out% %source% %sample_index%
%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head option$$

$subhead name$$
Is the name of the option we are setting.
It must be a valid
$cref/option_name/option_table/Conventions/option_name/$$.

$subhead value$$
Is the new
$cref/option_value/option_table/Conventions/option_value/$$
for the specified option name.

$subhead Warning$$
If $icode option_name$$ is $code parent_node_id$$,
the set of $cref model_variables$$ will change and you must
re-run the $cref init_command$$.

$head table_out$$
The $icode table_out$$ syntax sets the values in $icode table_out$$
using the values specified by $icode source$$.
If this table exists before the command,
the values originally in the table are lost.

$subhead start_var$$
If $icode table_out$$ is $code start_var$$,
the $cref/start_var/start_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$subhead scale_var$$
If $icode table_out$$ is $code scale_var$$,
the $cref/scale_var/scale_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$subhead truth_var$$
If $icode table_out$$ is $code truth_var$$,
the $cref/truth_var/truth_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$head source$$
The set command $icode source$$ must be one of the following
(and not be the same as $icode table_out$$):

$subhead prior_mean$$
If $icode source$$ is $code prior_mean$$,
the mean of the priors is used for the values in $icode table_out$$.
In this case $icode sample_index$$ is not present.

$subhead fit_var$$
If $icode source$$ is $code fit_var$$,
the results of the previous fit is used for the values in $icode table_out$$.
In this case $icode sample_index$$ is not present.

$subhead start_var$$
If $icode source$$ is $code start_var$$,
the contents of this table are copied to $icode table_out$$.
This is useful when the user creates the $code start_var$$ table
and wants the $code scale_var$$ table to have the same values.
In this case, $icode table_out$$ would be $code scale_var$$.

$subhead sample$$
If $icode source$$ is $code sample$$,
$icode sample_index$$ must be present.
In this case the $cref model_variables$$ in the sample table,
and corresponding to the specified sample index,
are used for the values in $icode table_out$$.

$children%example/get_started/set_command.py
%$$
$head Example$$
The files
$cref set_command.py$$ and $cref set_command.py$$
contains examples and tests using this command.

$end
*/

// ----------------------------------------------------------------------------
void set_option_command(
	sqlite3*                                        db           ,
	const CppAD::vector<option_struct>&  option_table ,
	const std::string&                              name         ,
	const std::string&                              value        )
{	using std::string;
	using CppAD::vector;

	// option_table contains all the valid option names with default values
	// for the names that are not in the option table, check name is valid
	bool found = false;
	for(size_t i = 0; i < option_table.size(); ++i)
		found |= option_table[i].option_name == name;
	if( ! found )
	{	string msg  = "set option name value: ";
		msg += "name = " + name + " is not a valid option name\n";
		error_exit(msg);
	}
	string table_name  = "option";
	string column_name = "option_name";
	CppAD::vector<string> option_name;
	get_table_column(db, table_name, column_name, option_name);
	found = false;
	for(size_t i = 0; i < option_name.size(); i++)
		found |= option_name[i] == name;
	string sql_cmd;
	if( found )
	{	sql_cmd  = "update option set option_value = '" + value + "' ";
		sql_cmd += "where option_name = '" + name + "'";
		exec_sql_cmd(db, sql_cmd);
	}
	else
	{	string option_id = CppAD::to_string( option_name.size() );
		sql_cmd  = "insert into option ";
		sql_cmd += "values ('" + option_id + "','";
		sql_cmd += name + "','" + value + "')";
		exec_sql_cmd(db, sql_cmd);
	}
	//
	return;
}
void set_command(
	const std::string&                     table_out    ,
	const std::string&                     source       ,
	const std::string&                     sample_index ,
	sqlite3*                               db           ,
	const CppAD::vector<double>&           prior_mean   )
{	using std::string;
	using CppAD::to_string;
	using CppAD::vector;
	//
	// string used for error messaging
	string msg;
	//
	if( table_out != "start_var"
	&&  table_out != "scale_var"
	&&  table_out != "truth_var" )
	{	msg  = "dismod_at set command table_out = ";
		msg += table_out + " is not one of the following: ";
		msg += "start_var, scale_var, truth_var";
		error_exit(msg);
	}
	if( source != "prior_mean"
	&&  source != "fit_var"
	&&  source != "start_var"
	&&  source != "sample" )
	{	msg  = "dismod_at set command source = ";
		msg += source + " is not one of the following: ";
		msg += "prior_mean, fit_var, start_var, sample";
		error_exit(msg);
	}
	if( source == "sample" && sample_index == "" )
	{	msg  = "dismod_at set command source = sample";
		msg += " sample_index is missing";
		error_exit(msg);
	}
	if( source == table_out )
	{	msg  = "dismod_at set command table_out and source are the same";
		error_exit(msg);
	}
	//
	string sql_cmd = "drop table if exists ";
	sql_cmd       += table_out;
	exec_sql_cmd(db, sql_cmd);
	//
	// create table_out
	string table_name_out = table_out;
	size_t n_var          = prior_mean.size();
	vector<string> col_name(1), col_type(1), row_value(n_var);
	vector<bool>   col_unique(1);
	col_name[0]       = table_out + "_value";
	col_type[0]       = "real";
	col_unique[0]     = false;
	//
	if( source == "prior_mean" )
	{	for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = CppAD::to_string( prior_mean[var_id] );
	}
	else if( source == "fit_var" )
	{	// fit_var_value
		vector<double> fit_var_value;
		string table_name_in  = "fit_var";
		string column_name    = "fit_var_value";
		get_table_column(
			db, table_name_in, column_name, fit_var_value
		);
		//
		// put it in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = to_string(fit_var_value[var_id]);
	}
	else if( source == "start_var" )
	{	// start_var_value
		vector<double> start_var_value;
		string table_name_in  = "start_var";
		string column_name    = "start_var_value";
		get_table_column(
			db, table_name_in, column_name, start_var_value
		);
		//
		// put it in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = to_string(start_var_value[var_id]);
	}
	else
	{	assert( source == "sample" );
		//
		// index
		size_t index = size_t ( std::atoi( sample_index.c_str() ) );
		//
		// var_value
		vector<double> var_value;
		string table_name_in  = "sample";
		string column_name    = "var_value";
		get_table_column(
			db, table_name_in, column_name, var_value
		);
		// n_sample
		if( var_value.size() % n_var != 0 )
		{	msg  = "sample table size not a multiple of number of variables";
			error_exit(msg);
		}
		size_t n_sample = var_value.size() / n_var;
		if( n_sample <= index )
		{	msg  = "dismod_at set command: sample_index >= number of samples";
			error_exit(msg);
		}
		//
		// put in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	size_t sample_id = index * n_var + var_id;
				row_value[var_id] = to_string(var_value[sample_id]);
		}
	}
	create_table(
		db, table_name_out, col_name, col_type, col_unique, row_value
	);
	return;
}

} // END_DISMOD_AT_NAMESPACE
