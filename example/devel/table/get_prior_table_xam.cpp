// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_prior_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_prior_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_prior_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);
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

	// sql commands
	// assume that density_id for uniform density is 0.
	const char* sql_cmd[] = {
		"create table prior("
			" prior_id      integer primary key,"
			" prior_name    text unique,"
			" density_id   integer,"
			" lower        real,"
			" upper        real,"
			" mean         real,"
			" std          real,"
			" eta          real,"
			" nu           real)",
	"insert into prior values(0, 'none', 0, null, null, 0.0, null, null, null)",
	"insert into prior values(1, 'rate', 1, 0.0,  1.0,  0.1, 1e-4, 1e-5, 5.0)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the prior table
	vector<dismod_at::prior_struct> prior_table =
			dismod_at::get_prior_table(db, density_table);
	ok  &= prior_table.size() == 2;
	//
	ok  &= prior_table[0].prior_name  == "none";
	ok  &= prior_table[0].density_id == 0;
	ok  &= prior_table[0].lower      == std::atof("-inf");
	ok  &= prior_table[0].upper      == std::atof("+inf");
	ok  &= prior_table[0].mean       == 0.0;
	//
	ok  &= prior_table[1].prior_name  == "rate";
	ok  &= prior_table[1].density_id == 1;
	ok  &= prior_table[1].lower      == 0.0;
	ok  &= prior_table[1].upper      == 1.0;
	ok  &= prior_table[1].mean       == 0.1;
	ok  &= prior_table[1].std        == 1e-4;
	ok  &= prior_table[1].eta        == 1e-5;
	ok  &= prior_table[1].nu         == 5.0;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
