// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_option_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_option_table: Example and Test$$

$srcfile%example/devel/table/get_option_table_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cppad/utility/to_string.hpp>
# include <dismod_at/get_option_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_option_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);
	//
	// option name and corresponding values
	// Testing with different order than in get_option_table
	// Furthermore, we have not included max_num_iter_fixed in list
	// so that its default value of 100 gets used.
	struct { const char* name; const char* value; } option_list[] = {
		"meas_std_effect",                  "add_std",
		// BEGIN_SORT_THIS_LINE_PLUS_1
		"accept_after_max_steps_fixed",     "6",
		"accept_after_max_steps_random",    "6",
		"age_avg_split",                    "1.0 2.0",
		"avgint_extra_columns",             "",
		"bound_frac_fixed",                 "1e-3",
		"bound_random",                     "3.0",
		"data_extra_columns",               "",
		"derivative_test_fixed",            "second-order",
		"derivative_test_random",           "first-order",
		"limited_memory_max_history_fixed", "15",
		"max_num_iter_random",              "50",
		"ode_step_size",                    "20.0",
		"parent_node_id",                   "1",
		"parent_node_name",                 "north_america",
		"print_level_fixed",                "5",
		"print_level_random",               "5",
		"quasi_fixed",                      "false",
		"random_seed",                      "123",
		"rate_case",                        "iota_zero_rho_zero",
		"tolerance_fixed",                  "1e-7",
		"tolerance_random",                 "1e-7",
		"warn_on_stderr",                   "true",
		"zero_sum_random",                  "iota  omega"
		// END_SORT_THIS_LINE_MINUS_1
	};
	size_t n_option = sizeof(option_list) / sizeof( option_list[0] );
	//
	// create table
	string sql_cmd =
	"create table option("
		" option_id       integer primary key,"
		" option_name     text unique,"
		" option_value    text"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd.c_str());
	//
	// put name, value pairs in table
	for(size_t option_id = 0; option_id < n_option; option_id++)
	{	sql_cmd  = "insert into option values(";
		sql_cmd += CppAD::to_string(option_id);
		sql_cmd += ", '";
		sql_cmd += option_list[option_id].name;
		if( string( option_list[option_id].value ) == "" )
			sql_cmd += "', null)";
		else
		{	sql_cmd += "', '";
			sql_cmd += option_list[option_id].value;
			sql_cmd += "')";
		}
		dismod_at::exec_sql_cmd(db, sql_cmd.c_str());
	}
	// get the option table
	vector<dismod_at::option_struct> option_table =
		dismod_at::get_option_table(db);
	//
	// check that this test sets all the possible values in the option table
	// (whiht max_num_iter_fixed left out)
	ok &=  n_option + 1 == option_table.size();
	//
	// check the table entries
	CppAD::vector<bool> found(n_option);
	for(size_t option_id = 0; option_id < n_option; option_id++)
		found[option_id] = false;
	for(size_t i = 0; i < n_option + 1; i++)
	{	size_t match = n_option;
		for(size_t option_id = 0; option_id < n_option; option_id++)
		{	if( option_table[i].option_name == option_list[option_id].name )
				match = option_id;
		}
		if( option_table[i].option_name == "max_num_iter_fixed" )
		{	ok &= match == n_option;
			ok &= option_table[i].option_value == "100";
		}
		else
		{	ok &= match < n_option;
			if( ok )
			{	ok &= ! found[match];
				found[match] = true;
				ok &= option_list[match].value == option_table[i].option_value;
			}
		}
	}
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
