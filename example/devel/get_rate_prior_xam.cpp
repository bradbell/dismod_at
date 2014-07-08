// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_rate_prior_xam.cpp$$
$spell
	xam
$$

$section C++ get_rate_prior: Example and Test$$
$index example, C++ get_rate_prior$$
$index get_rate_prior, C++ example$$

$code
$verbatim%example/devel/get_rate_prior_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/dismod_at.hpp>

bool get_rate_prior_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;	

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = { 
	"create table rate_prior("
		" rate_prior_id integer primary key,"
		" rate_id      integer,"
		" is_parent    integer,"
		" smooth_id    integer)",
	//                 rate_prior_id, rate_id,  is_parent,   smooth_id
	"insert into rate_prior values(0,       0,          1,           0)",
	"insert into rate_prior values(1,       1,          1,           0)",
	"insert into rate_prior values(2,       2,          1,           0)",
	"insert into rate_prior values(3,       3,          1,           0)",
	"insert into rate_prior values(4,       0,          0,           1)",
	"insert into rate_prior values(5,       1,          0,           1)",
	"insert into rate_prior values(6,       2,          0,           1)",
	"insert into rate_prior values(7,       3,          0,           1)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the rate_prior table
	vector<dismod_at::rate_prior_struct> 
		rate_prior = dismod_at::get_rate_prior(db);
	ok  &= rate_prior.size() == 8;
	//
	for(size_t i = 0; i < 8; i++)
	{	ok &= rate_prior[i].rate_id   == int(i % 4);
		ok &= rate_prior[i].is_parent == int( i < 4 );
		ok &= rate_prior[i].smooth_id == int( i >= 4 );
	}
 	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
