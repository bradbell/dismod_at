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
$begin smooth_info$$
$spell
	sqlite
	mulstd
	s_info
	dage
	dtime
	const
	CppAD
	struct
$$

$section Extract and Organize Information for One Smoothing$$

$head Syntax$$

$subhead Default Constructor$$
$codei%smooth_info %s_default%()
%$$

$subhead Normal Constructor$$
$codei%smooth_info %s_info%(
	%age_table%, %time_table%, %smooth_id%, %smooth_table%, %smooth_grid_table%
)
%$$

$subhead Testing Constructor$$
$codei%smooth_info %s_test%(
	%age_table%, %time_table%, %age_id%, %time_id%,
	%value_prior_id%, %dage_prior_id%, %dtime_prior_id%, %const_value%,
	%mulstd_value%, %mulstd_dage%, %mulstd_dtime%
)
%$$

$subhead Assignment$$
$icode%s_default% = %s_info%
%$$

$subhead Member Functions$$
$icode%n_age%     = %s_info%.age_size()
%$$
$icode%n_time%    = %s_info%.time_size()
%$$
$icode%a_id%      = %s_info%.age_id(%i%)
%$$
$icode%t_id%      = %s_info%.time_id(%j%)
%$$
$icode%c_value%   = %s_info%.const_value(%i%, %j%)
%$$
$icode%i_type%    = %s_info%.%type%_prior_id(%i%, %j%)
%$$
$icode%m_type%    = %s_info%.mulstd_%type%()
%$$

$head Purpose$$
Extracts, combines, and organizes the information for one smoothing from
the $cref/smooth/smooth_table/$$ and $cref/smooth_grid/smooth_grid_table/$$
tables.

$head Checks in Normal Constructor$$
$list number$$
Checks the $code smooth_grid$$ table
$cref/rectangular grid/smooth_grid_table/Rectangular Grid/$$ assumption.
$lnext
For each $cref/smooth_id/smooth_table/smooth_id/$$,
check that the $cref smooth_table$$ values
$cref/n_age/smooth_table/n_age/$$ and
$cref/n_time/smooth_table/n_time/$$ are the number if age values
and number of time values in the corresponding rectangular grid in
$cref smooth_grid_table$$.
$lnext
Sets
$cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$
($cref/dtime_prior_id/smooth_grid_table/dtime_prior_id/$$)
to $code DISMOD_AT_NULL_SIZE_T$$ for the maximum age (time) points.
$lnext
Checks that
$cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$
($cref/dtime_prior_id/smooth_grid_table/dtime_prior_id/$$)
is not $code DISMOD_AT_NULL_SIZE_T$$
for points that are not at maximum age (time).
$lend

$head s_info$$
In all its uses, except during construction,
this object has prototype
$codei%
	const smooth_info %s_info%
%$$
The meaning of the corresponding constructor arguments are specified below:

$subhead age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the $cref/age_table/get_age_table/$$.

$subhead time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the $cref/time_table/get_time_table/$$.

$subhead smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the $cref/smooth_id/smooth_grid_table/smooth_id/$$ for the
smoothing that $icode s_info$$ corresponds to.

$subhead smooth_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_struct>& %smooth_table%
%$$
and is the $cref/smooth_table/get_smooth_table/$$.

$subhead smooth_grid_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_grid_struct>& %smooth_grid_table%
%$$
and is the $cref/smooth_grid_table/get_smooth_grid/$$.

$subhead s_info$$
This result has type $code smooth_info$$.
All of the functions calls in the syntax above are $code const$$; i.e.,
they do not modify $icode s_info$$.

$head s_test$$
This constructor is used for testing purposes only.
The meaning of its arguments are specified below:

$subhead age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the $cref/age_table/get_age_table/$$.

$subhead time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the $cref/time_table/get_time_table/$$.

$subhead age_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %age_id%
%$$
It specifies the age grid indices; i.e.
$codei%
	%s_info%.age_id(%i%) = %age_id%[%i%]
%$$

$subhead time_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %time_id%
%$$
It specifies the time grid indices; i.e.
$codei%
	%s_info%.time_id(%j%) = %time_id%[%j%]
%$$

$subhead type_prior_id$$
For $icode type$$ equal to
$code value$$, $code dage$$ and $code dtime$$
these arguments have prototype
$codei%
const CppAD::vector<size_t>& %value_prior_id%, %dage_prior_id%, %dtime_prior_id%
%$$
They specify the prior grid indices; i.e.
$codei%
	%s_info%.value_prior_id(%i%, %j%) = %value_prior_id%[%i%*%n_time% + %j%]
	%s_info%.dage_prior_id(%i%, %j%)  =  %dage_prior_id%[%i%*%n_time% + %j%]
	%s_info%.dtime_prior_id(%i%, %j%) = %dtime_prior_id%[%i%*%n_time% + %j%]
%$$
where $icode%n_time% = %time_id%.size()%$$.


$subhead mulstd_type$$
For $icode type$$ equal to
$code value$$, $code dage$$ and $code dtime$$
these arguments have prototype
$codei%
	const size_t %mulstd_value%, %mulstd_dage%, %mulstd_dtime%
%$$
They specify the prior indices for the multiplies; i.e.,
$codei%
	%s_info%.mulstd_value()  = %mulstd_value%
	%s_info%.mulstd_dage()   = %mulstd_dage%
	%s_info%.mulstd_dtime()  = %mulstd_dtime%
%$$

$head s_default$$
This is the default constructor. It can be used to create
an empty $code smooth_info$$ object that is later set equal
to another $code weight_info$$ object.
This is useful when creating vectors of such objects.

$head n_age$$
This result has prototype
$codei%
	size_t %n_age%
%$$
and is the number of age values for this smoothing.

$head n_time$$
This result has prototype
$codei%
	size_t %n_time%
%$$
and is the number of time values for this smoothing.

$head i$$
The argument $icode i$$ has prototype
$codei%
	size_t %i%
%$$
and is the age index; $icode%i% < %n_age%$$.

$head j$$
The argument $icode j$$ has prototype
$codei%
	size_t %j%
%$$
and is the time index; $icode%j% < %n_time%$$.

$head a_id$$
This return value has prototype
$codei%
	size_t %a_id%
%$$
and is the $th i$$ $cref/age_id/smooth_grid_table/age_id/$$
for this smoothing and increases with $icode i$$; i.e.,
for $icode%i% < %n_age%-2%$$
$codei%
	%s_info%.age_id(%i%) < %s_info%.age_id(%i%+1)
%$$

$head t_id$$
This return value has prototype
$codei%
	size_t %t_id%
%$$
and is the $th j$$ $cref/time_id/smooth_grid_table/time_id/$$
for this smoothing and increases with $icode j$$; i.e.,
for $icode%j% < %n_time%-2%$$
$codei%
	%s_info%.time_id(%j%) < %s_info%.time_id(%j%+1)
%$$

$head c_value$$
This return value has prototype
$codei%
	double %c_value%
%%$$
If it is $code nan$$, then $icode i_value$$ (see below) specifies
the prior distribution for the value at this grid point.
If $icode c_value$$ is not $code nan$$,
the value at this grid point is constrained to be $icode c_value$$.

$head i_type$$
For $icode type$$ equal to
$code value$$, $code dage$$ and $code dtime$$
these return values have prototypes
$codei%
	size_t %i_value%, %i_dage%, %i_dtime%
%$$
and are the
$cref/value_prior_id/smooth_grid_table/value_prior_id/$$,
$cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$, and
$cref/dtime_prior_id/smooth_grid_table/dtime_prior_id/$$
corresponding to age index $icode i$$ and time index $icode j$$.
If $icode i_value$$ is (is not) $code DISMOD_AT_NULL_SIZE_T$$,
$icode c_value$$ is not (is) $code nan$$.

$head m_type$$
For $icode type$$ equal to
$code value$$, $code dage$$ and $code dtime$$
these return values have prototypes
$codei%
	size_t %m_value%, %m_dage%, %m_dtime%
%$$
and are the
$cref/mulstd_value_prior_id/smooth_table/mulstd_value_prior_id/$$,
$cref/mulstd_dage_prior_id/smooth_table/mulstd_dage_prior_id/$$, and
$cref/mulstd_dtime_prior_id/smooth_table/mulstd_dtime_prior_id/$$
for this smoothing.

$children%example/devel/table/smooth_info_xam.cpp
%$$
$head Example$$
The file $cref smooth_info_xam.cpp$$ contains an example that uses
this function.

$end
*/
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>

namespace {
	struct key_id {
		double key;
		size_t id;
	};
	void unique_insert_sort(
		CppAD::vector<key_id>&  vec     ,
		key_id&                 element )
	{	size_t n = vec.size();
		size_t i = 0;
		while( i < n && vec[i].key < element.key )
		{	assert( vec[i].id != element.id );
			i++;
		}
		if( i == n )
		{	vec.push_back(element);
			return;
		}
		if( vec[i].key == element.key )
		{	assert( vec[i].id == element.id );
			return;
		}
		vec.push_back( vec[n-1] );
		size_t j = n - 1;
		while( j > i )
		{	vec[j] = vec[j-1];
			j--;
		}
		vec[i] = element;
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

size_t smooth_info::age_size(void) const
{	return age_id_.size(); }
size_t smooth_info::time_size(void) const
{	return time_id_.size(); }
//
size_t smooth_info::age_id(size_t i) const
{	return age_id_[i]; }
size_t smooth_info::time_id(size_t j) const
{	return time_id_[j]; }
//
size_t smooth_info::value_prior_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return value_prior_id_[ i * time_id_.size() + j];
}
size_t smooth_info::dage_prior_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return dage_prior_id_[ i * time_id_.size() + j];
}
size_t smooth_info::dtime_prior_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return dtime_prior_id_[ i * time_id_.size() + j];
}
double smooth_info::const_value(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return const_value_[ i * time_id_.size() + j];
}
//
size_t smooth_info::mulstd_value(void) const
{	return mulstd_value_; }
size_t smooth_info::mulstd_dage(void)  const
{	return mulstd_dage_; }
size_t smooth_info::mulstd_dtime(void) const
{	return mulstd_dtime_; }

// Assigmnet Operator
void smooth_info::operator=(const smooth_info& s_info)
{	age_id_          = s_info.age_id_;
	time_id_         = s_info.time_id_;
	value_prior_id_  = s_info.value_prior_id_;
	dage_prior_id_   = s_info.dage_prior_id_;
	dtime_prior_id_  = s_info.dtime_prior_id_;
	const_value_     = s_info.const_value_;
	mulstd_value_    = s_info.mulstd_value_;
	mulstd_dage_     = s_info.mulstd_dage_;
	mulstd_dtime_    = s_info.mulstd_dtime_;
}


// Default constructor
smooth_info::smooth_info(void)
:
age_id_(0),
time_id_(0),
value_prior_id_(0),
dage_prior_id_(0),
dtime_prior_id_(0),
const_value_(0),
mulstd_value_(0),
mulstd_dage_(0),
mulstd_dtime_(0)
{ }


// Testing Constructor
smooth_info::smooth_info(
	const CppAD::vector<double>& age_table      ,
	const CppAD::vector<double>& time_table     ,
	const CppAD::vector<size_t>& age_id         ,
	const CppAD::vector<size_t>& time_id        ,
	const CppAD::vector<size_t>& value_prior_id ,
	const CppAD::vector<size_t>& dage_prior_id  ,
	const CppAD::vector<size_t>& dtime_prior_id ,
	const CppAD::vector<double>& const_value    ,
	size_t                       mulstd_value   ,
	size_t                       mulstd_dage    ,
	size_t                       mulstd_dtime   )
{	age_id_          = age_id;
	time_id_         = time_id;
	value_prior_id_  = value_prior_id;
	dage_prior_id_   = dage_prior_id;
	dtime_prior_id_  = dtime_prior_id;
	const_value_     = const_value;
	mulstd_value_    = mulstd_value;
	mulstd_dage_     = mulstd_dage;
	mulstd_dtime_    = mulstd_dtime;
# ifndef NDEBUG
	for(size_t i = 1; i < age_id.size(); i++)
		assert( age_table[age_id[i-1]] < age_table[age_id[i]] );
	for(size_t j = 1; j < time_id.size(); j++)
		assert( time_table[time_id[j-1]] < time_table[time_id[j]] );
# endif
}

// Normal Constructor
smooth_info::smooth_info(
	const CppAD::vector<double>&             age_table         ,
	const CppAD::vector<double>&             time_table        ,
	size_t                                   smooth_id         ,
	const CppAD::vector<smooth_struct>&      smooth_table      ,
	const CppAD::vector<smooth_grid_struct>& smooth_grid_table )
{	size_t i, j, id;
	using std::string;
	using CppAD::to_string;
	//
	// for erorr messaging
	string msg, table_name;
	size_t row_id;
	size_t null_size_t = DISMOD_AT_NULL_SIZE_T;

	// only priors in the smooth_table are the multiplier priors
	mulstd_value_   = smooth_table[smooth_id].mulstd_value_prior_id;
	mulstd_dage_    = smooth_table[smooth_id].mulstd_dage_prior_id;
	mulstd_dtime_   = smooth_table[smooth_id].mulstd_dtime_prior_id;

	// determine the age_id_ and time_id_ vectors for this smooth_id
	assert( age_id_.size() == 0 );
	assert( time_id_.size() == 0 );
	CppAD::vector<key_id> age_vec, time_vec;
	size_t n_smooth_grid = smooth_grid_table.size();
	for(i = 0; i < n_smooth_grid; i++)
	{	if( smooth_grid_table[i].smooth_id == int( smooth_id ) )
		{	key_id element;
			//
			element.id   = smooth_grid_table[i].age_id;
			element.key  = age_table[element.id];
			unique_insert_sort( age_vec,  element );
			//
			element.id   = smooth_grid_table[i].time_id;
			element.key  = time_table[element.id];
			unique_insert_sort( time_vec,  element );
		}
	}
	// number of age and time points for this smoothing
	size_t n_age  = age_vec.size();
	if( n_age != size_t( smooth_table[smooth_id].n_age ) )
	{	table_name = "smooth";
		msg  = "In smooth_grid table with smooth_id = " + to_string(smooth_id);
		msg += "\nnumber of unique age values is " + to_string(n_age) + ".\n";
		msg +=  "In smooth table, corresponding n_age = ";
		msg += to_string( smooth_table[smooth_id].n_age ) + ".";
		error_exit(msg, table_name, smooth_id);
	}
	size_t n_time = time_vec.size();
	if( n_time != size_t( smooth_table[smooth_id].n_time ) )
	{	table_name = "smooth";
		msg  = "In smooth_grid table with smooth_id = " + to_string(smooth_id);
		msg += "\nnumber of unique time values is " + to_string(n_time) + ".\n";
		msg +=  "In smooth table, corresponding n_time = ";
		msg += to_string( smooth_table[smooth_id].n_time ) + ".";
		error_exit(msg, table_name, smooth_id);
	}

	// age ids in order of increasing age for this smoothing
	age_id_.resize(n_age);
	for(i = 0; i < n_age; i++)
		age_id_[i] = age_vec[i].id;
	// time ids in order of increasing time for this smoothing
	time_id_.resize(n_time);
	for(j = 0; j < n_time; j++)
		time_id_[j] = time_vec[j].id;

	// set smoothing priors and count number of times each
	// age, time pair appears for this smooth_id
	CppAD::vector<size_t> count(n_age * n_time );
	value_prior_id_.resize( n_age * n_time );
	dage_prior_id_.resize ( n_age * n_time );
	dtime_prior_id_.resize( n_age * n_time );
	const_value_.resize   ( n_age * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_smooth_grid; i++)
	{	if( smooth_grid_table[i].smooth_id == int( smooth_id ) )
		{	id           = smooth_grid_table[i].age_id;
			size_t j_age = n_age;
			for(j = 0; j < n_age; j++ )
				if( id == age_id_[j] )
					j_age = j;
			assert( j_age < n_age );
			//
			id            = smooth_grid_table[i].time_id;
			size_t j_time = n_time;
			for(j = 0; j < n_time; j++ )
				if( id == time_id_[j] )
					j_time = j;
			assert( j_time < n_time );
			size_t index = j_age * n_time + j_time;
			count[index]++;
			//
			value_prior_id_[index] = smooth_grid_table[i].value_prior_id;
			dage_prior_id_[index]  = smooth_grid_table[i].dage_prior_id;
			dtime_prior_id_[index] = smooth_grid_table[i].dtime_prior_id;
			const_value_[index]    = smooth_grid_table[i].const_value;
			//
			// set dage_prior_id_ to null for the last age
			// (may not be null in the smoth_grid table).
			if( j_age == n_age - 1)
				dage_prior_id_[index] = null_size_t;
			//
			// check dage_piror_id is not null for other ages
			if( j_age != n_age -1 && dage_prior_id_[index] == null_size_t )
			{	table_name = "smooth_grid";
				row_id     = i;
				msg  = "age_id = " + to_string( age_id_[j_age] );
				msg += " is not the maximum age for smooth_id = ";
				msg += to_string(smooth_id) + " but dage_prior_id is null ";
				error_exit(msg, table_name, row_id);
			}
			//
			// set dtime_prior_id_ to null for the last time
			// (may not be null in smooth_grid_table).
			if( j_time == n_time - 1)
				 dtime_prior_id_[index] = null_size_t;
			//
			// check dtime_prior_id_ is not null for other times
			if( j_time != n_time -1 && dtime_prior_id_[index] == null_size_t )
			{	table_name = "smooth_grid";
				row_id     = i;
				msg  = "time_id = " + to_string( time_id_[j_time] );
				msg += " is not the maximum time for smooth_id = ";
				msg += to_string(smooth_id) + " but dtime_prior_id is null ";
				error_exit(msg, table_name, row_id);
			}
		}
	}

	// make sure each age, time pair appears once
	for(i = 0; i < n_age * n_time; i++)
	{	if( count[i] != 1 )
		{	size_t j_time = i % n_time;
			size_t j_age  = (i - j_time) / n_time;
			table_name = "smooth_grid";
			row_id     = null_size_t;
			msg  = "smooth_id = " + to_string( smooth_id );
			msg += ", age_id = " + to_string( age_id_[j_age] );
			msg += ", time_id = " + to_string( time_id_[j_time] );
			msg += ": appears " + to_string(count[i]) + " times";
			msg += " (should be one time). ";
			error_exit(msg, table_name, row_id);
		}
	}

}

} // END_DISMOD_AT_NAMESPACE
