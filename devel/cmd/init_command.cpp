/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-20 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/init_command.hpp>
# include <dismod_at/set_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin init_command$$
$spell
	sim
	avgint
	init
	var
	dismod
	initializes
$$

$section The Initialize Command$$

$head Syntax$$
$codei%dismod_at %database% init%$$

$head Purpose$$
This command initializes the data flow.
To be specific, it begins by deleting any existing
$cref/output tables/data_flow/Output Tables by Table Name/$$,
except for the $cref log_table$$,
and then creates new versions of the following tables:
$table
$cref age_avg_table$$       $cnext $title age_avg_table$$     $rnext
$cref var_table$$           $cnext $title var_table$$         $rnext
$cref data_subset_table$$   $cnext $title data_subset_table$$ $rnext
$cref start_var_table$$     $cnext $title start_var_table$$   $rnext
$cref scale_var_table$$     $cnext $title scale_var_table$$
$tend

$head Changing Values$$
You must re-run this command when ever any of the
$cref input$$ tables is changed.
The option table is an exception to this rule; see
$cref/changing values/option_table/Conventions/Changing Values/$$.

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head age_avg_table$$
A new $cref age_avg_table$$ is created when this command is run.
In fact, this table is recreated when any command is run except for the
python commands and the $cref set_command$$.


$head var_table$$
A new $cref var_table$$ is created with the information
that maps a $cref/var_id/var_table/var_id/$$
to its meaning in terms of the
$cref model_variables$$.

$head data_subset_table$$
A new $cref data_subset_table$$ is created.
This makes explicit exactly which rows of the data table are used.

$head start_var_table$$
A new $cref start_var_table$$ is created using the
means of the priors for the model variables.

$head scale_var_table$$
A new $cref scale_var_table$$ is created using the
means of the priors for the model variables.

$head Deleted Tables$$
The following tables are deleted (if they exist) when the
$code init$$ command is run:
$cref/depend_var/depend_var_table/$$,
$cref/fit_var/fit_var_table/$$,
$cref/truth_var/truth_var_table/$$,
$cref/fit_data_subset/fit_data_subset_table/$$,
$cref/data_sim/data_sim_table/$$,
$cref/sample/sample_table/$$,
$cref/predict/predict_table/$$.

$children%example/get_started/init_command.py
%$$
$head Example$$
The file $cref init_command.py$$ contains an example and test
using this command.

$end
*/

// ----------------------------------------------------------------------------
void init_command(
	sqlite3*                                         db                  ,
	const CppAD::vector<double>&                     prior_mean          ,
	const CppAD::vector<data_subset_struct>&         data_subset_obj     ,
	const pack_info&                                 pack_object         ,
	const db_input_struct&                           db_input            ,
	const size_t&                                    parent_node_id      ,
	const child_info&                                child_object        ,
	const CppAD::vector<smooth_info>&                s_info_vec          )
{	using std::string;
	using CppAD::to_string;
	using CppAD::vector;

	// -----------------------------------------------------------------------
	const char* drop_list[] = {
		"var",
		"depend_var",
		"fit_var",
		"scale_var",
		"start_var",
		"truth_var",
		//
		"data_subset",
		"fit_data_subset",
		//
		"data_sim",
		"sample",
		"predict"
	};
	size_t n_drop = sizeof( drop_list ) / sizeof( drop_list[0] );
	string sql_cmd;
	for(size_t i = 0; i < n_drop; i++)
	{	sql_cmd = "drop table if exists ";
		sql_cmd += drop_list[i];
		exec_sql_cmd(db, sql_cmd);
	}
	// -----------------------------------------------------------------------
	// start_var table
	string table_out    = "start_var";
	string source       = "prior_mean";
	string sample_index = "";
	set_command(table_out, source, sample_index, db, prior_mean);
	// -----------------------------------------------------------------------
	// scale_var table
	table_out    = "scale_var";
	source       = "prior_mean";
	sample_index = "";
	set_command(table_out, source, sample_index, db, prior_mean);
	// -----------------------------------------------------------------------
	// data_subset table
	string table_name = "data_subset";
	size_t n_subset   = data_subset_obj.size();
	vector<string> col_name(1), col_type(1), row_value(n_subset);
	vector<bool>   col_unique(1);
	col_name[0]       = "data_id";
	col_type[0]       = "integer";
	col_unique[0]     = true;
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int data_id    = data_subset_obj[subset_id].original_id;
		row_value[subset_id] = to_string( data_id );
	}
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// -----------------------------------------------------------------------
	// create var table
	size_t n_row = pack_object.size();
	size_t n_col = 11;
	table_name   = "var";
	col_name.resize(n_col);
	col_type.resize(n_col);
	col_unique.resize(n_col);
	row_value.clear();
	row_value.resize(n_col * n_row);
	//
	col_name[0]   = "var_type";
	col_type[0]   = "text";
	col_unique[0] = false;
	//
	col_name[1]   = "smooth_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "age_id";
	col_type[2]   = "integer";
	col_unique[2] = false;
	//
	col_name[3]   = "time_id";
	col_type[3]   = "integer";
	col_unique[3] = false;
	//
	col_name[4]   = "node_id";
	col_type[4]   = "integer";
	col_unique[4] = false;
	//
	col_name[5]   = "rate_id";
	col_type[5]   = "integer";
	col_unique[5] = false;
	//
	col_name[6]   = "integrand_id";
	col_type[6]   = "integer";
	col_unique[6] = false;
	//
	col_name[7]   = "covariate_id";
	col_type[7]   = "integer";
	col_unique[7] = false;
	//
	col_name[8]   = "mulcov_id";
	col_type[8]   = "integer";
	col_unique[8] = false;
	//
	col_name[9]   = "group_id";
	col_type[9]   = "integer";
	col_unique[9] = false;
	//
	col_name[10]   = "subgroup_id";
	col_type[10]   = "integer";
	col_unique[10] = false;
	//
	// mulstd variables
	size_t n_smooth = db_input.smooth_table.size();
	size_t offset = DISMOD_AT_NULL_SIZE_T;
	size_t var_id = DISMOD_AT_NULL_SIZE_T;
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	offset  = pack_object.mulstd_offset(smooth_id, k);
			if( offset != DISMOD_AT_NULL_SIZE_T )
			{	var_id  = offset;
# ifndef NDEBUG
				for(size_t j = 0; j < n_col; j++)
					assert( row_value[ n_col * var_id + j ] == "" );
# endif
				// var_type
				string var_type;
				switch(k)
				{	case 0:
					var_type = "mulstd_value";
					break;

					case 1:
					var_type = "mulstd_dage";
					break;

					case 2:
					var_type = "mulstd_dtime";
					break;

					default:
					assert(false);
				}
				row_value[ n_col * var_id + 0] = var_type;

				// smooth_id
				row_value[ n_col * var_id + 1] = to_string( smooth_id );
			}
		}
	}
	// rate variables
	size_t n_rate  = db_input.rate_table.size();
	size_t n_child = child_object.child_size();
	size_t n_var, n_time, node_id;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	if( child_id == n_child )
				node_id = parent_node_id;
			else
				node_id = child_object.child_id2node_id(child_id);
			//
			pack_info::subvec_info info;
			info      = pack_object.node_rate_value_info(rate_id, child_id);
			size_t smooth_id = info.smooth_id;
			if( smooth_id == DISMOD_AT_NULL_SIZE_T )
				n_var = 0;
			else
			{	offset    = info.offset;
				n_var     = info.n_var;
				n_time    = db_input.smooth_table[smooth_id].n_time;
# ifndef NDEBUG
				size_t n_age = db_input.smooth_table[smooth_id].n_age;
				assert( n_var == n_age * n_time );
# endif
			}
			for(size_t index = 0; index < n_var; index++)
			{	size_t i       = index / n_time;
				size_t j       = index % n_time;
				var_id         = offset + index;
				size_t age_id  = s_info_vec[smooth_id].age_id(i);
				size_t time_id = s_info_vec[smooth_id].time_id(j);
				//
				// fit_var_value
# ifndef NDEBUG
				for(size_t k = 0; k < n_col; k++)
					assert( row_value[ n_col * var_id + k ] == "" );
# endif
				row_value[n_col * var_id + 0] = "rate"; // var_type
				row_value[n_col * var_id + 1] = to_string( smooth_id );
				row_value[n_col * var_id + 2] = to_string( age_id );
				row_value[n_col * var_id + 3] = to_string( time_id );
				row_value[n_col * var_id + 4] = to_string( node_id );
				row_value[n_col * var_id + 5] = to_string( rate_id );
			}
		}
	}
	//
	// mulcov_table
	const vector<mulcov_struct>& mulcov_table( db_input.mulcov_table );
	size_t n_mulcov        = mulcov_table.size();
	size_t n_integrand     = db_input.integrand_table.size();
	//
	// initialize counters for different types of covariate multipliers
	vector<size_t> count_group_meas_value(n_integrand);
	vector<size_t> count_group_meas_noise(n_integrand);
	vector<size_t> count_subgroup_meas_value(n_integrand);
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	count_group_meas_value[integrand_id]    = 0;
		count_group_meas_noise[integrand_id]    = 0;
		count_subgroup_meas_value[integrand_id] = 0;
	}
	vector<size_t> count_group_rate_value(n_rate);
	vector<size_t> count_subgroup_rate_value(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	count_group_rate_value[rate_id]    = 0;
		count_subgroup_rate_value[rate_id] = 0;
	}
	//
	// group covariate multipliers
	vector<size_t> smooth_id_loop(2), n_subgroup(2);
	for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
	{	mulcov_type_enum mulcov_type;
		mulcov_type              = mulcov_table[mulcov_id].mulcov_type;
		size_t rate_id           = mulcov_table[mulcov_id].rate_id;
		size_t integrand_id      = mulcov_table[mulcov_id].integrand_id;
		size_t covariate_id      = mulcov_table[mulcov_id].covariate_id;
		size_t group_id          = mulcov_table[mulcov_id].group_id;
		size_t first_subgroup_id = pack_object.first_subgroup_id(group_id);
		//
		smooth_id_loop[0]        = mulcov_table[mulcov_id].group_smooth_id;
		smooth_id_loop[1]        = mulcov_table[mulcov_id].subgroup_smooth_id;
		n_subgroup[0]            = 1;
		n_subgroup[1]            = pack_object.subgroup_size(group_id);
		//
		for(size_t loop = 0; loop < 2; ++loop)
		for(size_t k = 0; k < n_subgroup[loop]; ++k)
		if( smooth_id_loop[loop] != DISMOD_AT_NULL_SIZE_T )
		{	size_t smooth_id = smooth_id_loop[loop];
			pack_info::subvec_info info;
			switch( mulcov_type )
			{
				case rate_value_enum:
				if( loop == 0 )
				{	info = pack_object.group_rate_value_info(
						rate_id, count_group_rate_value[rate_id]++
					);
				}
				else
				{	info = pack_object.subgroup_rate_value_info(
						rate_id, count_subgroup_rate_value[rate_id], k
					);
					if( k == n_subgroup[loop] - 1 )
						++count_subgroup_rate_value[rate_id];
				}
				break;

				case meas_value_enum:
				if( loop == 0 )
				{	info = pack_object.group_meas_value_info(
						integrand_id, count_group_meas_value[integrand_id]++
					);
				}
				else
				{	info = pack_object.subgroup_meas_value_info(
						integrand_id,
						count_subgroup_meas_value[integrand_id],
						k
					);
					if( k == n_subgroup[loop] - 1 )
						++count_subgroup_meas_value[integrand_id];
				}
				break;

				case meas_noise_enum:
				assert(loop == 0 );
				info = pack_object.group_meas_noise_info(
					integrand_id, count_group_meas_noise[integrand_id]++
				);
				break;

				default:
				assert(false);
			}
			//
			assert( smooth_id == info.smooth_id);
			offset    = info.offset;
			n_var     = info.n_var;
			n_time    = db_input.smooth_table[smooth_id].n_time;
	# ifndef NDEBUG
			size_t n_age = db_input.smooth_table[smooth_id].n_age;
			assert( n_var == n_age * n_time );
	# endif
			for(size_t index = 0; index < n_var; index++)
			{	size_t i        = index / n_time;
				size_t j        = index % n_time;
				var_id          = offset + index;
				size_t age_id   = s_info_vec[smooth_id].age_id(i);
				size_t time_id  = s_info_vec[smooth_id].time_id(j);
	# ifndef NDEBUG
				for(size_t ell = 0; ell < n_col; ++ell)
					assert( row_value[ n_col * var_id + ell ] == "" );
	# endif
				if( mulcov_type == rate_value_enum )
				{	row_value[n_col * var_id + 0]  = "mulcov_rate_value";
					row_value[n_col * var_id + 5] = to_string( rate_id );
				}
				else if( mulcov_type == meas_value_enum )
				{	row_value[n_col * var_id + 0]  = "mulcov_meas_value";
					row_value[n_col * var_id + 6] = to_string( integrand_id );
				}
				else if( mulcov_type == meas_noise_enum )
				{	row_value[n_col * var_id + 0]  = "mulcov_meas_noise";
					row_value[n_col * var_id + 6] = to_string( integrand_id );
				}
				else assert(false);
				//
				row_value[n_col * var_id + 1] = to_string( smooth_id );
				row_value[n_col * var_id + 2] = to_string( age_id );
				row_value[n_col * var_id + 3] = to_string( time_id );
				row_value[n_col * var_id + 7] = to_string( covariate_id );
				row_value[n_col * var_id + 8] = to_string( mulcov_id );
				if( loop == 0 )
					row_value[n_col * var_id + 9] = to_string( group_id );
				else
				{	size_t subgroup_id = first_subgroup_id + k;
					row_value[n_col * var_id + 10] = to_string( subgroup_id );
				}
			}
		}
	}
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// ----------------------------------------------------------------------
	return;
}

} // END_DISMOD_AT_NAMESPACE
