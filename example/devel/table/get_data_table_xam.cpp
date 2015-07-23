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
$begin get_data_table_xam.cpp$$
$spell
	data
	xam
$$

$section C++ get_data_table: Example and Test$$

$code
$verbatim%example/devel/table/get_data_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_data_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table data("
		" data_id        integer primary key,"
		" integrand_id   integer,"
		" density_id     integer,"
		" node_id        integer,"
		" weight_id      integer,"
		" hold_out       integer,"
		" meas_value     real,"
		" meas_std       real,"
		" age_lower      real,"
		" age_upper      real,"
		" time_lower     real,"
		" time_upper     real,"
		" x_0            real,"
		" x_1            real,"
		" c_data_source  text)",
	"insert into data values("
		"0,"                       // data_id
		"1,"                       // integrand_id
		"1,"                       // density_id (not uniform)
		"3,"                       // node_id
		"4,"                       // weight_id
		"1,"                       // hold_out
		"1e-4,"                    // meas_value
		"1e-5,"                    // meas_std
		"10.0,"                    // age_lower
		"90.0,"                    // age_upper
		"2000,"                    // time_lower
		"2010,"                    // time_upper
		"0.5,"                     // x_0  (sex)
		"1000,"                    // x_1  (income)
		"'www.healthdata.org')"    // c_data_source
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the data table
	size_t n_covariate = 2;
	double age_min     = 0.0;
	double age_max     = 100.0;
	double time_min    = 1900.;
	double time_max    = 2015.;
	vector<dismod_at::data_struct> data_table = dismod_at::get_data_table(
		db, n_covariate, age_min, age_max, time_min, time_max
	);
	ok  &= data_table.size() == 1;
	//
	ok  &= data_table[0].integrand_id      == 1;
	ok  &= data_table[0].density_id        == 1;
	ok  &= data_table[0].node_id           == 3;
	ok  &= data_table[0].weight_id         == 4;
	ok  &= data_table[0].hold_out          == 1;
	ok  &= data_table[0].meas_value        == 1e-4;
	ok  &= data_table[0].meas_std          == 1e-5;
	ok  &= data_table[0].age_lower         == 10.0;
	ok  &= data_table[0].age_upper         == 90.0;
	ok  &= data_table[0].time_lower        == 2000.0;
	ok  &= data_table[0].time_upper        == 2010.0;
	ok  &= data_table[0].x[0]              == 0.5;
	ok  &= data_table[0].x[1]              == 1000.0;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
