// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

/*
$begin varaible_vec$$
$spell
	integrands
	op
	mulstd
	dage
	dtime
	var
	dismod
	vec
	mulcov
	const
	CppAD
	struct
	enum
$$

$section The Variable Vector Class$$

$head Syntax$$
$codei%dismod_at::variable_vec<%Float%> %var%(
	%parent_node_id%, %n_smooth%, %n_integrand%,
	%node_table%, %data_table%, %mulcov_table%
)
%$$
$icode%var%.%op%_%type%_mulstd(%mulstd%, %smooth_id%)
%$$

$head Float$$
The type $icode Float$$ is $code double$$ or $code CppAD::AD<double>$$.

$head parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the 
$cref/parent_node_id/run_table/parent_node_id/$$.

$head n_smooth$$
This argument has prototype
$codei%
	size_t %n_smooth%
%$$
and is the number of smoothing; i.e., the size of
$cref/smooth_table/get_smooth_table/smooth_table/$$.

$head n_integrand$$
This argument has prototype
$codei%
	size_t %n_integrand%
%$$
and is the number of integrands; i.e., the size of
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head node_table$$
This argument has prototype
$code%
	const CppAD::vector<node_struct>&  %node_table%
%$$
and is the $cref/node_table/get_node_table/node_table/$$.
Only the $code parent$$ field of each
$cref/node_struct/get_node_table/node_struct/$$ is used.

$head data_table$$
This argument has prototype
$code%
	const CppAD::vector<data_struct>&  %data_table%
%$$
and is the $cref/data_table/get_data_table/data_table/$$.
Only the $code node_id$$ field of each
$cref/data_struct/get_data_table/data_struct/$$ is used.

$head mulcov_table$$
This argument has prototype
$code%
	const CppAD::vector<mulcov_struct>&  %mulcov_table%
%$$
and is the $cref/mulcov_table/get_mulcov_table/mulcov_table/$$.

$head op_type_mulstd$$
These functions  set and get the standard deviation multiplier.

$subhead op$$
The $icode op$$ is either $code set$$ or $code get$$ depending
on if we are setting or getting $icode mulstd$$.

$subhead type$$
The $icode type$$ of smoothing multiplier is
$code value$$, $code dage$$, or $code dtime$$.

$subhead mulstd$$
For $icode%op% = set%$$, the argument $icode mulstd$$ has prototype
$codei%
	const %Float%& %mulstd%
%$$
For $icode%op% = get%$$, it has prototype
$codei%
	const %Float%& %mulstd%
%$$
and is the standard deviation multiplier.

$subhead smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the 
$cref/smooth_id/smooth_table/smooth_id/$$.


$children%
	example/devel/model/variable_vec_xam.cpp
%$$
$head Example$$
The file $cref variable_vec_xam.cpp$$ contains
and example and test of $code variable_vec$$.
It returns true for success and false for failure.

$end
*/

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
variable_vec<Float>::variable_vec(
	size_t                                parent_node_id    ,
	size_t                                n_smooth          ,
	size_t                                n_integrand       ,
	const CppAD::vector<node_struct>&     node_table        ,
	const CppAD::vector<data_struct>&     data_table        ,
	const CppAD::vector<mulcov_struct>&   mulcov_table 
) :
parent_node_id_( parent_node_id ) ,
n_smooth_( n_smooth )             ,
n_integrand_( n_integrand )
{	using std::string;

	size_t n_node      = node_table.size();
	size_t n_data      = data_table.size();
	// size_t n_mulcov    = mulcov_table.size();

	// child_node_id_
	assert( node_id_.size() == 0 );
	node_id_.push_back( parent_node_id );
	for(size_t i = 0; i < n_node; i++)
	{	if( node_table[i].parent == parent_node_id )
			node_id_.push_back(i);
	}

	// data_id2child_index_
	data_id2node_index_.resize( data_table.size() );
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	int    node_id = data_table[data_id].node_id;
		size_t j       = 0;
		bool   more    = node_id != -1;
		while( more )
		{	bool found = false;
			while( (! found) &&  j < node_id_.size() )
			{	found = node_id == node_id_[j];
				if( ! found )
					j++;
			}
			if( found )
				more = false;
			else 
			{	node_id = node_table[node_id].parent;
				more    = node_id != -1;
				j       = 0;
			}
		}
		if( node_id == -1 )
			data_id2node_index_[data_id] = size_t(-1);
		else
			data_id2node_index_[data_id] = j;
	}

	// mulstd

# if 0
	// rate_mean_mulcov_
	rate_mean_mulcov_.resize( number_rate_enum );
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	assert( rate_mean_mulcov_[rate_id].size() == 0 );
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match = mulcov_table[mulcov_id].mulcov_type == "rate_mean";
			match     &= mulcov_table[mulcov_id].rate_id == rate_id;
			if( match )
			{	mulcov_pair info;
				info.covariate_id = mulcov_table[mulcov_id].covariate_id;
				info.smooth_id    = mulcov_table[mulcov_id].smooth_id;
				for(size_t i = 0; i < rate_mean_mulcov_.size(); i++)
				{	size_t tmp = rate_mean_mulcov_[i].covariate_id;	
					if(info.covariate_id == tmp )
					{	string table_name = "mulcov";
						string message = 
							"This rate_mean covaraite appears twice";
						table_error_exit(table_name, mulcov_id, message);
					}
				}
				rate_mean_mulcov_.pushback(info);
			}
		}
	}

	// meas_mean_mulcov_
	meas_mean_mulcov_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	assert( meas_mean_mulcov_[integrand_id].size() == 0 );
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match = mulcov_table[mulcov_id].mulcov_type == "meas_mean";
			match &= mulcov_table[mulcov_id].integrand_id == integrand_id;
			if( match )
			{	mulcov_pair info;
				info.covariate_id = mulcov_table[mulcov_id].covariate_id;
				info.smooth_id    = mulcov_table[mulcov_id].smooth_id;
				for(size_t i = 0; i < meas_mean_mulcov_.size(); i++)
				{	size_t tmp = meas_mean_mulcov_[i].covariate_id;	
					if(info.covariate_id == tmp )
					{	string table_name = "mulcov";
						string message = 
							"This meas_mean covaraite appears twice";
						table_error_exit(table_name, mulcov_id, message);
					}
				}
				meas_mean_mulcov_.pushback(info);
			}
		}
	}

	// meas_std_mulcov_
	meas_std_mulcov_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	assert( meas_std_mulcov_[integrand_id].size() == 0 );
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match = mulcov_table[mulcov_id].mulcov_type == "meas_std";
			match &= mulcov_table[mulcov_id].integrand_id == integrand_id;
			if( match )
			{	mulcov_pair info;
				info.covariate_id = mulcov_table[mulcov_id].covariate_id;
				info.smooth_id    = mulcov_table[mulcov_id].smooth_id;
				for(size_t i = 0; i < meas_std_mulcov_.size(); i++)
				{	size_t tmp = meas_std_mulcov_[i].covariate_id;	
					if(info.covariate_id == tmp )
					{	string table_name = "mulcov";
						string message = 
							"This meas_std covaraite appears twice";
						table_error_exit(table_name, mulcov_id, message);
					}
				}
				meas_std_mulcov_.pushback(info);
			}
		}
	}
# endif

	// set size of vec_
	vec_.resize( n_smooth_ );
}

template <class Float> void variable_vec<Float>::
set_value_mulstd( const Float& mulstd, size_t smooth_id )
{	assert( smooth_id < n_smooth_ );
	vec_[ offset_value_mulstd_ + smooth_id] = mulstd; 
}
template <class Float> void variable_vec<Float>::
get_value_mulstd( Float& mulstd, size_t smooth_id )
{	assert( smooth_id < n_smooth_ );
	mulstd = vec_[ offset_value_mulstd_ + smooth_id];
}




// instantiations
template class variable_vec<double>;
template class variable_vec< CppAD::AD<double> >;

} // END DISMOD_AT_NAMESPACE
