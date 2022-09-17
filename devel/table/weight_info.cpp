// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin weight_info$$
$spell
	const
	CppAD
	struct
	w_info
$$

$section Extract and Organize Information for One Weighting Function$$

$head Syntax$$
$codei%weight_info %w_constant%()
%$$
$codei%weight_info %w_info%(
	%age_table%, %time_table%, %weight_id%, %weight_table%, %weight_grid_table%
)
%$$
$codei%weight_info %w_test%(
	%age_table%, %time_table%, %age_id%, %time_id%, %weight%
)
%$$
$icode%w_default% = %w_info%
%$$
$icode%n_age%     = %w_info%.age_size()
%$$
$icode%n_time%    = %w_info%.time_size()
%$$
$icode%a_id%      = %w_info%.age_id(%i%)
%$$
$icode%t_id%      = %w_info%.time_id(%j%)
%$$
$icode%w%         = %w_info%.weight(%i%, %j%)
%$$

$head Purpose$$
Extracts the information for one weighting from
the $cref weight_grid_table$$.
In addition, this routine checks the $code weight_info$$ table
$cref/rectangular grid/weight_grid_table/Rectangular Grid/$$ assumption.

$head w_constant$$
The default constructor creates a constant weighting.
It is also used to create a $code weight_info$$ object that is later set equal
to another $code weight_info$$ object.
This is useful when creating vectors of such objects.

$head w_info$$
In all its uses, except during construction,
this object has prototype
$codei%
	const weight_info %w_info%
%$$
The mean of the corresponding constructor arguments are specified below:

$head w_test$$
This object is the result of the testing constructor and can be used
the same as $icode w_info$$.


$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the $cref/age_table/get_age_table/$$.

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the $cref/time_table/get_time_table/$$.

$head weight_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %weight_table%
%$$
and is the $cref/weight_table/get_weight_table/$$.

$head weight_grid_table$$
This argument has prototype
$codei%
	const CppAD::vector<weight_grid_struct>& %weight_grid_table%
%$$
an is the $cref weight_grid_table$$.

$head age_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %age_id%
%$$
It specifies the age grid indices; i.e.
$codei%
	%w_info%.age_id(%i%) = %age_id%[%i%]
%$$

$head time_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %time_id%
%$$
It specifies the time grid indices; i.e.
$codei%
	%w_info%.time_id(%i%) = %time_id%[%i%]
%$$

$head weight$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %weight%
%$$
It specifies the weight grid values in row major order; i.e.
$codei%
	%w_info%.weight(%i%, %j%) = %time_id%[%i%*%n_time% + %j%]
%$$

$head n_age$$
This result has prototype
$codei%
	size_t %n_age%
%$$
and is the number of age values for this weighting.

$head n_time$$
This result has prototype
$codei%
	size_t %n_time%
%$$
and is the number of time values for this weighting.

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
and is the $th i$$ $cref/age_id/weight_grid_table/age_id/$$
for this weighting and increases with $icode i$$; i.e.,
for $icode%i% < %n_age%-2%$$
$codei%
	%w_info%.age_id(%i%) < %w_info%.age_id(%i%+1)
%$$

$head t_id$$
This return value has prototype
$codei%
	size_t %t_id%
%$$
and is the $th j$$ $cref/time_id/weight_grid_table/time_id/$$
for this weighting and increases with $icode j$$; i.e.,
for $icode%j% < %n_time%-2%$$
$codei%
	%w_info%.time_id(%j%) < %w_info%.time_id(%j%+1)
%$$

$head w$$
This return value has prototype
$codei%
	double %w%
%$$
and is the weighting for the corresponding age and time indices.

$children%example/devel/table/weight_info_xam.cpp
%$$
$head Example$$
The file $cref weight_info_xam.cpp$$ contains an example that uses
this function.

$end
*/
# include <dismod_at/weight_info.hpp>

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

size_t weight_info::age_size(void) const
{	return age_id_.size(); }
size_t weight_info::time_size(void) const
{	return time_id_.size(); }
//
size_t weight_info::age_id(size_t i) const
{	return age_id_[i]; }

size_t weight_info::time_id(size_t j) const
{	return time_id_[j]; }
//
double weight_info::weight(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return weight_[ i * time_id_.size() + j];
}

// Assignment operator
void weight_info::operator=(const weight_info& w_info)
{	// resize to zero so CppAD::vector assingments are legal
	age_id_.resize(0);
	time_id_.resize(0);
	weight_.resize(0);
	//
	age_id_  =  w_info.age_id_;
	time_id_ = w_info.time_id_;
	weight_  = w_info.weight_;
}

// Default constructor (constant weighting)
weight_info::weight_info(void)
:
age_id_(1)  ,
time_id_(1) ,
weight_(1)
{	age_id_[0]  = 0;
	time_id_[0] = 0;
	weight_[0]  = 1.0;
}

// Testing Constructor
weight_info::weight_info(
	const CppAD::vector<double>& age_table  ,
	const CppAD::vector<double>& time_table ,
	const CppAD::vector<size_t>& age_id     ,
	const CppAD::vector<size_t>& time_id    ,
	const CppAD::vector<double>& weight     )
{	age_id_  = age_id;
	time_id_ = time_id;
	weight_  = weight;
# ifndef NDEBUG
	for(size_t i = 1; i < age_id.size(); i++)
		assert( age_table[age_id[i-1]] < age_table[age_id[i]] );
	for(size_t j = 1; j < time_id.size(); j++)
		assert( time_table[time_id[j-1]] < time_table[time_id[j]] );
# endif
}
// Normal Constructor
weight_info::weight_info(
	const CppAD::vector<double>&             age_table         ,
	const CppAD::vector<double>&             time_table        ,
	size_t                                   weight_id         ,
	const CppAD::vector<weight_struct>&      weight_table      ,
	const CppAD::vector<weight_grid_struct>& weight_grid_table )
{	size_t i, j, id;

	using std::cerr;
	using std::endl;
	using std::string;

	// determine the vector age_id and time_id vectors for this weight_id
	assert( age_id_.size() == 0 );
	assert( time_id_.size() == 0 );
	size_t n_weight = weight_grid_table.size();
	CppAD::vector<key_id> age_vec, time_vec;
	for( i = 0; i < n_weight; i++)
	{	if( weight_grid_table[i].weight_id == int(weight_id) )
		{	key_id element;
			//
			element.id  = weight_grid_table[i].age_id;
			element.key = age_table[element.id];
			unique_insert_sort( age_vec,  element);
			//
			element.id  = weight_grid_table[i].time_id;
			element.key = time_table[element.id];
			unique_insert_sort( time_vec, element );
		}
	}

	// number of age and time points for this weighting
	size_t n_age  = age_vec.size();
	if( n_age != size_t( weight_table[weight_id].n_age ) )
	{	cerr << "In weight_table with weight_id = " << weight_id
		<< ": n_age = " << weight_table[weight_id].n_age << endl;
		cerr << "In weight_grid_table with weight_id = " << weight_id
		<< ": n_age = " << n_age << endl;
		std::exit(1);
	}
	size_t n_time = time_vec.size();
	if( n_time != size_t( weight_table[weight_id].n_time ) )
	{	cerr << "In weight_table with weight_id = " << weight_id
		<< ": n_time = " << weight_table[weight_id].n_time << endl;
		cerr << "In weight_grid_table with weight_id = " << weight_id
		<< ": n_time = " << n_time << endl;
		std::exit(1);
	}

	// age ids in order of increasing age for this weighting
	age_id_.resize(n_age);
	for(i = 0; i < n_age; i++)
		age_id_[i] = age_vec[i].id;
	// time ids in order of increasing time for this weighting
	time_id_.resize(n_time);
	for(j = 0; j < n_time; j++)
		time_id_[j] = time_vec[j].id;

	// set weight_ and count number of times each
	// age, time pair appears for this weight_id
	CppAD::vector<size_t> count(n_age * n_time );
	weight_.resize(n_age  * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_weight; i++)
	{	if( weight_grid_table[i].weight_id == int(weight_id) )
		{	id           = weight_grid_table[i].age_id;
			size_t j_age = n_age;
			for(j = 0; j < n_age; j++ )
				if( id == age_id_[j] )
					j_age = j;
			assert( j_age < n_age );
			//
			id            = weight_grid_table[i].time_id;
			size_t j_time = n_time;
			for(j = 0; j < n_time; j++ )
				if( id == time_id_[j] )
					j_time = j;
			assert( j_time < n_time );
			size_t index = j_age * n_time + j_time;
			count[index]++;
			//
			weight_[index] = weight_grid_table[i].weight;
		}
	}

	// make sure each age, time pair appears once
	for(i = 0; i < n_age * n_time; i++)
	{	if( count[i] != 1 )
		{	size_t j_time = i % n_time;
			size_t j_age  = (i - j_time) / n_time;
			cerr << "weight_grid table with weight_id = " << weight_id
			<< endl << "age_id = " << age_id_[j_age]
			<< ", time_id = " << time_id_[j_time] << " appears "
			<< count[i] << " times (not 1 time)." << endl;
		}
	}
}


} // END_DISMOD_AT_NAMESPACE
