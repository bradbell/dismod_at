// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_mulcov_table_xam.cpp$$
$spell
	mulcov
	xam
$$

$section C++ get_mulcov_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cppad/utility/to_string.hpp>
# include <dismod_at/get_mulcov_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool get_mulcov_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	// assume that density_id for uniform density is 0.
	const char* sql_cmd[] = {
		"create table mulcov("
			" mulcov_id          integer primary key,"
			" mulcov_type        text,"
			" rate_id            integer,"
			" integrand_id       integer,"
			" covariate_id       integer,"
			" group_id           integer,"
			" group_smooth_id    integer,"
			" subgroup_smooth_id integer)",
		"insert into mulcov values(0, 'meas_value', null, 1,  2, 3, 4, 5 )",
		"insert into mulcov values(1, 'rate_value', 1, null,  6, 7, 8, 9 )"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	int n_subgroup = 8; // greater than maximum group_id above
	vector<dismod_at::subgroup_struct> subgroup_table(n_subgroup);
	for(int subgroup_id = 0; subgroup_id < n_subgroup; ++subgroup_id)
	{	string subgroup_name = CppAD::to_string(subgroup_id);
		subgroup_table[subgroup_id].subgroup_name = subgroup_name;
		subgroup_table[subgroup_id].group_id      = subgroup_id;
		subgroup_table[subgroup_id].group_name    = subgroup_name;
	}

	// get the mulcov table
	vector<dismod_at::mulcov_struct>
		mulcov_table = dismod_at::get_mulcov_table(db, subgroup_table);
	ok  &= mulcov_table.size() == 2;
	//
	ok  &= mulcov_table[0].mulcov_type  == dismod_at::meas_value_enum;
	ok  &= mulcov_table[1].mulcov_type  == dismod_at::rate_value_enum;
	//
	ok  &= mulcov_table[0].rate_id == DISMOD_AT_NULL_INT;
	ok  &= mulcov_table[1].rate_id == 1;
	//
	ok  &= mulcov_table[0].integrand_id == 1;
	ok  &= mulcov_table[1].integrand_id == DISMOD_AT_NULL_INT;
	//
	ok  &= mulcov_table[0].covariate_id == 2;
	ok  &= mulcov_table[1].covariate_id == 6;
	//
	ok  &= mulcov_table[0].group_id == 3;
	ok  &= mulcov_table[1].group_id == 7;
	//
	ok  &= mulcov_table[0].group_smooth_id == 4;
	ok  &= mulcov_table[1].group_smooth_id == 8;
	//
	ok  &= mulcov_table[0].subgroup_smooth_id == 5;
	ok  &= mulcov_table[1].subgroup_smooth_id == 9;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
