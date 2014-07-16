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
$begin smooth_grid$$
$spell
	const
	CppAD
	struct
$$

$section Extract Information for One Weighting Function$$

$head Syntax$$
$codei%smooth_grid %s%(
	%smooth_id%,
	%smooth_grid_table%
)%$$
$icode%n_age%   = %sg%.age_size()
%$$
$icode%n_time%  = %sg%.time_size()
%$$
$icode%a_id%    = %sg%.age_id(%i%)
%$$
$icode%t_id%    = %sg%.time_id(%j%)
%$$
$icode%v_like%  = %sg%.value_like_id(%i%, %j%)
%$$
$icode%a_like%  = %sg%.dage_like_id(%i%, %j%)
%$$
$icode%t_like%  = %sg%.dtime_like_id(%i%, %j%)
%$$

$head Purpose$$
Extracts the information for one smoothing from
the $cref smooth_grid_table$$.

$head Assumptions$$
$list number$$
Checks the $code smooth_grid$$ table
$cref/rectangular grid/smooth_grid/Rectangular Grid/$$ assumption.
$lnext
Checks that 
$cref/dage_like_id/smooth_grid/d_age_like_id/$$ is $code -1$$
for the maximum age points and only the maximum age points.
$lnext
Checks that 
$cref/dtime_like_id/smooth_grid/d_time_like_id/$$ is $code -1$$
for the maximum time points and on the maximum time points.
$lend

$head Constructor$$

$subhead smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the $cref/smooth_id/smooth_grid_table/smooth_id/$$ for the
smoothing that $icode sg$$ corresponds to.

$subhead smooth_grid_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_grid_struct>& %smooth_grid_table%
%$$
an is the $cref smooth_grid_table$$.

$subhead sg$$
This result has type $code smooth_grid$$.
All of the functions calls in the syntax above are $code const$$; i.e.,
they do not modify $icode sg$$.

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
	%sg%.age_id(%i%) < %sg%.age_id(%i%+1)
%$$.

$head t_id$$
This return value has prototype
$codei%
	size_t %t_id%
%$$
and is the $th j$$ $cref/time_id/smooth_grid_table/time_id/$$ 
for this smoothing and increases with $icode j$$; i.e.,
for $icode%j% < %n_time%-2%$$
$codei%
	%sg%.time_id(%j%) < %sg%.time_id(%j%+1)
%$$.

$head v_like$$
This return value has prototype
$codei%
	size_t %v_like%
%$$
and is the 
$cref/value_like_id/smooth_grid/value_like_id/$$ 
for the corresponding age and time indices.

$head a_like$$
This return value has prototype
$codei%
	size_t %a_like%
%$$
and is the 
$cref/dage_like_id/smooth_grid/dage_like_id/$$ 
for the corresponding age and time indices.

$head a_like$$
This return value has prototype
$codei%
	size_t %a_like%
%$$
and is the 
$cref/dage_like_id/smooth_grid/dage_like_id/$$ 
for the corresponding age and time indices.

$head t_like$$
This return value has prototype
$codei%
	size_t %t_like%
%$$
and is the 
$cref/dtime_like_id/smooth_grid/dtime_like_id/$$ 
for the corresponding age and time indices.

$end
*/
# include <dismod_at/dismod_at.hpp>

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

size_t smooth_grid::age_size(void) const
{	return age_id_.size(); }
size_t smooth_grid::time_size(void) const
{	return time_id_.size(); }
//
size_t smooth_grid::age_id(size_t i) const
{	return age_id_[i]; }

size_t smooth_grid::time_id(size_t j) const
{	return time_id_[j]; }
//
size_t smooth_grid::value_like_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return value_like_id_[ i * time_id_.size() + j]; 
}
size_t smooth_grid::dage_like_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return dage_like_id_[ i * time_id_.size() + j]; 
}
size_t smooth_grid::dtime_like_id(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return dtime_like_id_[ i * time_id_.size() + j]; 
}

// Constructor
smooth_grid::smooth_grid(
	size_t                                   smooth_id         ,
	const CppAD::vector<smooth_grid_struct>& smooth_grid_table )
{	size_t i, j, id;

	using std::cerr;
	using std::endl;
	using std::string;

	// determine the vector age_id and time_id vectors for this smooth_id
	assert( age_id_.size() == 0 );
	assert( time_id_.size() == 0 );
	size_t n_smooth = smooth_grid_table.size();
	for( i = 0; i < n_smooth; i++)
	{	if( smooth_grid_table[i].smooth_id == smooth_id )
		{	id  = smooth_grid_table[i].age_id;
			unique_insert_sort( age_id_,  id );
			id  = smooth_grid_table[i].time_id;
			unique_insert_sort( time_id_, id );
		}
	}

	// number of age and time points for this smoothing
	size_t n_age  = age_id_.size();
	size_t n_time = time_id_.size();

	// set likelihoods and count number of times each 
	// age, time pair appears for this smooth_id
	CppAD::vector<size_t> count(n_age * n_time );
	value_like_id_.resize(n_age  * n_time );
	dage_like_id_.resize(n_age  * n_time );
	dtime_like_id_.resize(n_age  * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_smooth; i++)
	{	if( smooth_grid_table[i].smooth_id == smooth_id )
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
			if( j_age == n_age -1 && dage_like_id_[index] != -1 )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "age_id = " << age_id_[j_age]
				<< " is maximum age for smooth_id = " << smooth_id
				<< endl << " but dage_like_id = " << dage_like_id_[index]
				<< " is not -1" << endl;
				exit(1);
			}
			if( j_age != n_age -1 && dage_like_id_[index] == -1 )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "age_id = " << age_id_[j_age]
				<< " is not maximum age for smooth_id = " << smooth_id
				<< endl << " but dage_like_id = -1 " << endl;
				exit(1);
			}
			if( j_time == n_time -1 && dtime_like_id_[index] != -1 )
			{	cerr << "smooth_grid table with smooth_grid_id = " << i
				<< endl << "time_id = " << time_id_[j_time]
				<< " is maximum time for smooth_id = " << smooth_id
				<< endl << " but dtime_like_id = " << dtime_like_id_[index]
				<< " is not -1" << endl;
				exit(1);
			}
			if( j_time != n_time -1 && dtime_like_id_[index] == -1 )
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
