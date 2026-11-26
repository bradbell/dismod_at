// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin weight_info_xam.cpp dev}

C++ weight_info: Example and Test
#################################

{xrst_literal
    // BEGIN C++
    // END C++
}

{xrst_end weight_info_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/get_weight_table.hpp>
# include <dismod_at/weight_info.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool weight_info_xam(void)
{
    bool   ok = true;
    using  std::string;
    using  CppAD::vector;

    string   file_name = "example.db";
    bool     new_file  = true;
    sqlite3* db        = dismod_at::open_connection(file_name, new_file);

    // data_table
    vector<dismod_at::data_struct>   data_table;
    //
    // avgint_table
    vector<dismod_at::avgint_struct> avgint_table(2);
    avgint_table[0].weight_id = 0;

    // sql commands
    const char* sql_cmd[] = {
    "create table age("
        " age_id         integer primary key,"
        " age            real)",
    "insert into age values(0, 100.0)",
    "insert into age values(1, 75.0)",
    "insert into age values(2, 50.0)",
    "insert into age values(3, 25.0)",
    "insert into age values(4, 0.0)",
    //
    "create table time("
        " time_id        integer primary key,"
        " time           real)",
    "insert into time values(0, 1990.0)",
    "insert into time values(1, 2010.0)",
    //
    "create table weight("
        " weight_id      integer primary key,"
        " weight_name    text unique,"
        " n_age          integer,"
        " n_time         integer)",
    "insert into weight values(0, 'my weight', 3, 2)",
    "insert into weight values(1, 'constant',  1, 1)",
    //
    "create table weight_grid("
        " weight_grid_id integer primary key,"
        " weight_id      integer, "
        " age_id         integer,"
        " time_id        integer,"
        " weight         real)",
    //                 weight_grid_id, weight_id,age_id,time_id, weight
    "insert into weight_grid values(0,         0,     0,      0,    0.50)",
    "insert into weight_grid values(1,         0,     2,      0,    0.75)",
    "insert into weight_grid values(2,         0,     4,      0,    1.00)",
    "insert into weight_grid values(3,         1,     0,      0,    2.00)",
    "insert into weight_grid values(4,         0,     0,      1,    1.00)",
    "insert into weight_grid values(5,         0,     2,      1,    1.25)",
    "insert into weight_grid values(6,         0,     4,      1,    1.50)",
    };
    size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
    for(size_t i = 0; i < n_command; i++)
        dismod_at::exec_sql_cmd(db, sql_cmd[i]);

    // get the age_grid table
    vector<double> age_table = dismod_at::get_age_table(db);

    // get the time_grid table
    vector<double> time_table = dismod_at::get_time_table(db);

    // get the weight_grid table
    vector<dismod_at::weight_struct>
        weight_table = dismod_at::get_weight_table(db);

    // get the weight_grid table
    vector<dismod_at::weight_grid_struct>
        weight_grid_table = dismod_at::get_weight_grid(
            db, data_table, avgint_table
    );

    // group the rows of the weight_grid table by weight_id
    size_t n_weight = weight_table.size();
    vector<size_t> grid_start, grid_order;
    dismod_at::weight_grid_group(
        n_weight, weight_grid_table, grid_start, grid_order
    );
    ok  &= grid_start.size() == n_weight + 1;
    ok  &= grid_start[0]     == 0;
    ok  &= grid_start[1]     == 6;
    ok  &= grid_start[2]     == 7;
    //
    // rows for weight_id zero are not all next to each other
    ok  &= grid_order.size() == 7;
    ok  &= grid_order[2]     == 2;
    ok  &= grid_order[3]     == 4;
    ok  &= grid_order[6]     == 3;

    // extract the weight_grid information
    size_t weight_id = 0;
    dismod_at::weight_info w_info(
        age_table, time_table, weight_id, weight_table, weight_grid_table,
        grid_start, grid_order
    );

    ok  &= w_info.age_size()  == 3;
    ok  &= w_info.time_size() == 2;
    //
    ok  &= w_info.age_id(0)    == 4;
    ok  &= w_info.age_id(1)    == 2;
    ok  &= w_info.age_id(2)    == 0;
    //
    ok  &= w_info.time_id(0)   == 0;
    ok  &= w_info.time_id(1)   == 1;
    //
    ok  &= w_info.weight(0,0)  == 1.00;
    ok  &= w_info.weight(1,0)  == 0.75;
    ok  &= w_info.weight(2,0)  == 0.50;
    ok  &= w_info.weight(0,1)  == 1.50;
    ok  &= w_info.weight(1,1)  == 1.25;
    ok  &= w_info.weight(2,1)  == 1.00;
    //
    // the constant weighting
    weight_id = 1;
    dismod_at::weight_info w_constant(
        age_table, time_table, weight_id, weight_table, weight_grid_table,
        grid_start, grid_order
    );
    ok  &= w_constant.age_size()  == 1;
    ok  &= w_constant.time_size() == 1;
    ok  &= w_constant.age_id(0)   == 0;
    ok  &= w_constant.time_id(0)  == 0;
    ok  &= w_constant.weight(0,0) == 2.00;
    //
    // close database and return
    sqlite3_close(db);
    return ok;
}
// END C++
