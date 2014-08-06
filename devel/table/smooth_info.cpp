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
$begin smooth_info$$
$spell
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
$codei%smooth_info %s_info%(%smooth_id%, %smooth_table%, %smooth_grid_table% )
%$$
$codei%smooth_info %s_info%(
	%age_id%,
	%time_id%,
	%value_like_id%,
	%dage_like_id%,
	%dtime_like_id%
	%mulstd_value%,
	%mulstd_dage%,
	%mulstd_dtime%,
)
%$$
$icode%n_age%   = %s_info%.age_size()
%$$
$icode%n_time%  = %s_info%.time_size()
%$$
$icode%a_id%    = %s_info%.age_id(%i%)
%$$
$icode%t_id%    = %s_info%.time_id(%j%)
%$$
$icode%i_type%  = %s_info%.%type%_like_id(%i%, %j%)
%$$
$icode%m_type%  = %s_info%.mulstd_%type%()
%$$

$head Purpose$$
Extracts the information for one smoothing from
the $cref smooth_grid_table$$.

$head Assumptions$$
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
Checks that 
$cref/dage_like_id/smooth_grid_table/dage_like_id/$$ is $code -1$$
for the maximum age points and only the maximum age points.
$lnext
Checks that 
$cref/dtime_like_id/smooth_grid_table/dtime_like_id/$$ is $code -1$$
for the maximum time points and on the maximum time points.
$lend

$head Constructor From Table$$

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
an is the $cref smooth_table$$.

$subhead smooth_grid_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_grid_struct>& %smooth_grid_table%
%$$
an is the $cref smooth_grid_table$$.

$subhead s_info$$
This result has type $code smooth_info$$.
All of the functions calls in the syntax above are $code const$$; i.e.,
they do not modify $icode s_info$$.

$head Testing Constructor$$
This constructor is used for testing purposes only:

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
	%s_info%.time_id(%i%) = %time_id%[%i%]
%$$ 

$subhead type_like_id$$
For $icode type$$ equal to 
$code value$$, $code dage$$ and $code dtime$$
these arguments have prototype
$codei%
const CppAD::vector<size_t>& %value_like_id%, %dage_like_id%, %dtime_like_id%
%$$
They specify the likelihood grid indices; i.e.
$codei%
	%s_info%.value_like_id(%i%, %j%) = %value_like_id%[%i%*%n_time% + %j%]
	%s_info%.dage_like_id(%i%, %j%)  = %dage_like_id%[%i%*%n_time% + %j%]
	%s_info%.dtime_like_id(%i%, %j%) = %dtime_like_id%[%i%*%n_time% + %j%]
%$$ 
where $icode%n_time% = %time_id%.size()%$$.


$subhead mulstd_type$$
For $icode type$$ equal to 
$code value$$, $code dage$$ and $code dtime$$
these arguments have prototype
$codei%
	const size_t %mulstd_value%, %mulstd_dage%, %mulstd_dtime%
%$$
They specify the likelihood indices for the multiplies; i.e.,
$codei%
	%s_info%.mulstd_value()  = %mulstd_value%
	%s_info%.mulstd_dage()   = %mulstd_dage%
	%s_info%.mulstd_dtime()  = %mulstd_dtime%
%$$ 

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

$head i, j$$
These arguments have prototype
$codei%
	size_t %i%, %j%
%%$$

$head i_type$$
For $icode type$$ equal to 
$code value$$, $code dage$$ and $code dtime$$
these return values have prototypes
$codei%
	size_t %i_value%, %i_dage%, %i_dtime%
%$$
and are the 
$cref/value_like_id/smooth_grid_table/value_like_id/$$,
$cref/dage_like_id/smooth_grid_table/dage_like_id/$$, and
$cref/dtime_like_id/smooth_grid_table/dtime_like_id/$$ 
corresponding to age index $icode i$$ and time index $icode j$$.

$head m_type$$
For $icode type$$ equal to 
$code value$$, $code dage$$ and $code dtime$$
these return values have prototypes
$codei%
	size_t %m_value%, %m_dage%, %m_dtime%
%$$
and are the 
$cref/mulstd_value/smooth_table/mulstd_value/$$,
$cref/mulstd_dage/smooth_table/mulstd_dage/$$, and
$cref/mulstd_dtime/smooth_table/mulstd_dtime/$$ 
for this smoothing.

$children%example/devel/table/smooth_info_xam.cpp
%$$
$head Example$$
The file $cref smooth_info_xam.cpp$$ contains an example that uses
this function.

$end
*/
# include <dismod_at/include/smooth_info.hpp>

namespace {
	void unique_insert_sort(
		CppAD::vector<size_t>& vec     ,
		size_t                 element )
	{	size_t n = vec.size();
		size_t i = 0;
		while( i < n && vec[i] < element )
			i++;
		if( i == n )
		{	vec.push_back(element);
			return;
		}
		if( vec[i] == element )
			return;
		vec.push_back( vec[n-1] );
		size_t j = n - 1;
		while( j > i )
			vec[j] = vec[j-1];
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
size_t smooth_info::value_like_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return value_like_id_[ i * time_id_.size() + j]; 
}
size_t smooth_info::dage_like_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return dage_like_id_[ i * time_id_.size() + j]; 
}
size_t smooth_info::dtime_like_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return dtime_like_id_[ i * time_id_.size() + j]; 
}
//
size_t smooth_info::mulstd_value(void) const
{	return mulstd_value_; }
size_t smooth_info::mulstd_dage(void)  const
{	return mulstd_dage_; }
size_t smooth_info::mulstd_dtime(void) const
{	return mulstd_dtime_; }
//
// Testing Constructor
smooth_info::smooth_info(
	const CppAD::vector<size_t>& age_id         ,
	const CppAD::vector<size_t>& time_id        ,
	const CppAD::vector<size_t>& value_like_id  ,
	const CppAD::vector<size_t>& dage_like_id   ,
	const CppAD::vector<size_t>& dtime_like_id  ,
	size_t                       mulstd_value   ,
	size_t                       mulstd_dage    ,
	size_t                       mulstd_dtime   )
	{	age_id_          = age_id;
		time_id_         = time_id;
		value_like_id_   = value_like_id;
		dage_like_id_    = dage_like_id;
		dtime_like_id_   = dtime_like_id;
		mulstd_value_    = mulstd_value;
		mulstd_dage_     = mulstd_dage;
		mulstd_dtime_    = mulstd_dtime;
# ifndef NDEBUG
		for(size_t i = 1; i < age_id.size(); i++)
			assert( age_id[i-1] < age_id[i] );
		for(size_t j = 1; j < time_id.size(); j++)
			assert( time_id[j-1] < time_id[j] );
# endif
	}
// Constructor
smooth_info::smooth_info(
	size_t                                   smooth_id         ,
	const CppAD::vector<smooth_struct>&      smooth_table      ,
	const CppAD::vector<smooth_grid_struct>& smooth_grid_table )
{	size_t i, j, id;

	using std::cerr;
	using std::endl;
	using std::string;

	// check that -1 is not a valid positive int
	assert( -1 == int( size_t(-1) ) );

	// only use of smooth_table is to determine multiplier likelihoods
	mulstd_value_   = smooth_table[smooth_id].mulstd_value;
	mulstd_dage_    = smooth_table[smooth_id].mulstd_dage;
	mulstd_dtime_   = smooth_table[smooth_id].mulstd_dtime;

	// determine the age_id_ and time_id_ vectors for this smooth_id
	assert( age_id_.size() == 0 );
	assert( time_id_.size() == 0 );
	size_t n_smooth = smooth_grid_table.size();
	for(i = 0; i < n_smooth; i++)
	{	if( smooth_grid_table[i].smooth_id == int( smooth_id ) )
		{	id  = smooth_grid_table[i].age_id;
			unique_insert_sort( age_id_,  id );
			id  = smooth_grid_table[i].time_id;
			unique_insert_sort( time_id_, id );
		}
	}

	// number of age and time points for this smoothing
	size_t n_age  = age_id_.size();
	size_t n_time = time_id_.size();

	// set smoothing likelihoods and count number of times each 
	// age, time pair appears for this smooth_id
	CppAD::vector<size_t> count(n_age * n_time );
	value_like_id_.resize(n_age  * n_time );
	dage_like_id_.resize(n_age  * n_time );
	dtime_like_id_.resize(n_age  * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_smooth; i++)
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
			value_like_id_[index] = smooth_grid_table[i].value_like_id;
			dage_like_id_[index]  = smooth_grid_table[i].dage_like_id;
			dtime_like_id_[index] = smooth_grid_table[i].dtime_like_id;
			//
			if( j_age == n_age -1 && dage_like_id_[index] != size_t(-1) )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "age_id = " << age_id_[j_age]
				<< " is maximum age for smooth_id = " << smooth_id
				<< endl << " but dage_like_id = " << dage_like_id_[index]
				<< " is not -1" << endl;
				exit(1);
			}
			if( j_age != n_age -1 && dage_like_id_[index] == size_t(-1) )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "age_id = " << age_id_[j_age]
				<< " is not maximum age for smooth_id = " << smooth_id
				<< endl << " but dage_like_id = -1 " << endl;
				exit(1);
			}
			if( j_time == n_time -1 && dtime_like_id_[index] != size_t(-1) )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "time_id = " << time_id_[j_time]
				<< " is maximum time for smooth_id = " << smooth_id
				<< endl << " but dtime_like_id = " << dtime_like_id_[index]
				<< " is not -1" << endl;
				exit(1);
			}
			if( j_time != n_time -1 && dtime_like_id_[index] == size_t(-1) )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "time_id = " << time_id_[j_time]
				<< " is not maximum time for smooth_id = " << smooth_id
				<< endl << " but dtime_like_id = -1 " << endl;
				exit(1);
			}
		}
	}

	// make sure each age, time pair appears once
	for(i = 0; i < n_age * n_time; i++)
	{	if( count[i] != 1 )
		{	size_t j_time = i % n_time;
			size_t j_age  = (i - j_time) / n_time;
			cerr << "smooth_grid table with smooth_id = " << smooth_id
			<< endl << "age_id = " << age_id_[j_age]
			<< ", time_id = " << time_id_[j_time] << " appears "
			<< count[i] << " times (not 1 time)." << endl;
			exit(1);
		}
	}

}

} // END_DISMOD_AT_NAMESPACE
