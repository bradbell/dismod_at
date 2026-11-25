// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// SPDX-FileContributor: 2026 Joseph Wagner
// ----------------------------------------------------------------------------
/*
{xrst_begin weight_info dev}

Extract and Organize Information for One Weighting Function
###########################################################

Syntax
******

| ``weight_info`` *w_constant* ()
| ``weight_grid_group`` (
| |tab| *n_weight* , *weight_grid_table* , *grid_start* , *grid_order*
| )
| ``weight_info`` *w_info* (
| |tab| *age_table* , *time_table* , *weight_id* , *weight_table* , *weight_grid_table* ,
| |tab| *grid_start* , *grid_order*
| )
| ``weight_info`` *w_test* (
| |tab| *age_table* , *time_table* , *age_id* , *time_id* , *weight*
| )
| *w_default* = *w_info*
| *n_age* = *w_info* . ``age_size`` ()
| *n_time* = *w_info* . ``time_size`` ()
| *a_id* = *w_info* . ``age_id`` ( *i* )
| *t_id* = *w_info* . ``time_id`` ( *j* )
| *w* = *w_info* . ``weight`` ( *i* , *j* )

Purpose
*******
Extracts the information for one weighting from
the :ref:`weight_grid_table-name` .
In addition, this routine checks the ``weight_info`` table
:ref:`weight_grid_table@Rectangular Grid` assumption.

w_constant
**********
The default constructor creates a constant weighting.
It is also used to create a ``weight_info`` object that is later set equal
to another ``weight_info`` object.
This is useful when creating vectors of such objects.

w_info
******
In all its uses, except during construction,
this object has prototype

    ``const weight_info`` *w_info*

The mean of the corresponding constructor arguments are specified below:

w_test
******
This object is the result of the testing constructor and can be used
the same as *w_info* .

age_table
*********
This argument has prototype

    ``const CppAD::vector<double>&`` *age_table*

and is the :ref:`age_table<get_age_table-name>` .

time_table
**********
This argument has prototype

    ``const CppAD::vector<double>&`` *time_table*

and is the :ref:`time_table<get_time_table-name>` .

weight_table
************
This argument has prototype

    ``const CppAD::vector<double>&`` *weight_table*

and is the :ref:`weight_table<get_weight_table-name>` .

weight_grid_table
*****************
This argument has prototype

    ``const CppAD::vector<weight_grid_struct>&`` *weight_grid_table*

an is the :ref:`weight_grid_table-name` .

weight_grid_group
*****************
This routine makes one pass over *weight_grid_table* and sets
*grid_start* and *grid_order* so that a ``weight_info`` constructor
only needs to visit the rows for its *weight_id* .
Call it once, and then use its results to construct
*w_info* for each *weight_id* .

n_weight
********
This argument has prototype

    ``size_t`` *n_weight*

and is the number of rows in the weight table.

grid_start
**********
This is an output of ``weight_grid_group`` and
an input to the *w_info* constructor.
It has prototype

    ``CppAD::vector<size_t>`` *grid_start*

Its size is *n_weight* + 1 and for *weight_id* less than *n_weight* ,

    *grid_start* [ *weight_id* +1] ``-`` *grid_start* [ *weight_id* ]

is the number of rows in *weight_grid_table* that have this *weight_id* .

grid_order
**********
This is an output of ``weight_grid_group`` and
an input to the *w_info* constructor.
It has prototype

    ``CppAD::vector<size_t>`` *grid_order*

For *k* greater than or equal *grid_start* [ *weight_id* ] and
less than *grid_start* [ *weight_id* +1] ,

    *grid_order* [ *k* ]

is the index in *weight_grid_table* of a row that has this *weight_id* .
For each *weight_id* , these indices are in increasing order.

age_id
******
This argument has prototype

    ``const CppAD::vector<size_t>&`` *age_id*

It specifies the age grid indices; i.e.

    *w_info* . ``age_id`` ( *i* ) = *age_id* [ *i* ]

time_id
*******
This argument has prototype

    ``const CppAD::vector<size_t>&`` *time_id*

It specifies the time grid indices; i.e.

    *w_info* . ``time_id`` ( *i* ) = *time_id* [ *i* ]

weight
******
This argument has prototype

    ``const CppAD::vector<double>&`` *weight*

It specifies the weight grid values in row major order; i.e.

    *w_info* . ``weight`` ( *i* , *j* ) = *time_id* [ *i* * *n_time* + *j* ]

n_age
*****
This result has prototype

    ``size_t`` *n_age*

and is the number of age values for this weighting.

n_time
******
This result has prototype

    ``size_t`` *n_time*

and is the number of time values for this weighting.

i
*
The argument *i* has prototype

    ``size_t`` *i*

and is the age index; *i* < *n_age* .

j
*
The argument *j* has prototype

    ``size_t`` *j*

and is the time index; *j* < *n_time* .

a_id
****
This return value has prototype

    ``size_t`` *a_id*

and is the *i*-th :ref:`weight_grid_table@age_id`
for this weighting and increases with *i* ; i.e.,
for *i* < *n_age* ``-2``

    *w_info* . ``age_id`` ( *i* ) < *w_info* . ``age_id`` ( *i* +1)

t_id
****
This return value has prototype

    ``size_t`` *t_id*

and is the *j*-th :ref:`weight_grid_table@time_id`
for this weighting and increases with *j* ; i.e.,
for *j* < *n_time* ``-2``

    *w_info* . ``time_id`` ( *j* ) < *w_info* . ``time_id`` ( *j* +1)

w
*
This return value has prototype

    ``double`` *w*

and is the weighting for the corresponding age and time indices.
{xrst_toc_hidden
    example/devel/table/weight_info_xam.cpp
}
Example
*******
The file :ref:`weight_info_xam.cpp-name` contains an example that uses
this function.

{xrst_end weight_info}
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
    {   size_t n = vec.size();
        size_t i = 0;
        while( i < n && vec[i].key < element.key )
        {   assert( vec[i].id != element.id );
            i++;
        }
        if( i == n )
        {   vec.push_back(element);
            return;
        }
        if( vec[i].key == element.key )
        {   assert( vec[i].id == element.id );
            return;
        }
        vec.push_back( vec[n-1] );
        size_t j = n - 1;
        while( j > i )
        {   vec[j] = vec[j-1];
            j--;
        }
        vec[i] = element;
    }
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

size_t weight_info::age_size(void) const
{   return age_id_.size(); }
size_t weight_info::time_size(void) const
{   return time_id_.size(); }
//
size_t weight_info::age_id(size_t i) const
{   return age_id_[i]; }

size_t weight_info::time_id(size_t j) const
{   return time_id_[j]; }
//
double weight_info::weight(size_t i, size_t j) const
{   assert( i < age_id_.size() );
    assert( j < time_id_.size() );
    return weight_[ i * time_id_.size() + j];
}

// Assignment operator
void weight_info::operator=(const weight_info& w_info)
{   // resize to zero so CppAD::vector assignments are legal
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
{   age_id_[0]  = 0;
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
{   age_id_  = age_id;
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
    const CppAD::vector<weight_grid_struct>& weight_grid_table ,
    const CppAD::vector<size_t>&             grid_start        ,
    const CppAD::vector<size_t>&             grid_order        )
{   size_t i, j, k, id;

    using std::cerr;
    using std::endl;
    using std::string;

    // determine the vector age_id and time_id vectors for this weight_id
    assert( age_id_.size() == 0 );
    assert( time_id_.size() == 0 );
    //
    // k_begin, k_end
    // grid_order[k] for k_begin <= k < k_end are the rows for this weight_id
    assert( grid_start.size() == weight_table.size() + 1 );
    size_t k_begin = grid_start[weight_id];
    size_t k_end   = grid_start[weight_id + 1];
    //
    // age_vec, time_vec
    CppAD::vector<key_id> age_vec, time_vec;
    for( k = k_begin; k < k_end; k++)
    {   i = grid_order[k];
        assert( weight_grid_table[i].weight_id == int(weight_id) );
        key_id element;
        //
        element.id  = weight_grid_table[i].age_id;
        element.key = age_table[element.id];
        unique_insert_sort( age_vec,  element);
        //
        element.id  = weight_grid_table[i].time_id;
        element.key = time_table[element.id];
        unique_insert_sort( time_vec, element );
    }

    // number of age and time points for this weighting
    size_t n_age  = age_vec.size();
    if( n_age != size_t( weight_table[weight_id].n_age ) )
    {   cerr << "In weight_table with weight_id = " << weight_id
        << ": n_age = " << weight_table[weight_id].n_age << endl;
        cerr << "In weight_grid_table with weight_id = " << weight_id
        << ": n_age = " << n_age << endl;
        std::exit(1);
    }
    size_t n_time = time_vec.size();
    if( n_time != size_t( weight_table[weight_id].n_time ) )
    {   cerr << "In weight_table with weight_id = " << weight_id
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
    for( k = k_begin; k < k_end; k++)
    {   i            = grid_order[k];
        id           = weight_grid_table[i].age_id;
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

    // make sure each age, time pair appears once
    for(i = 0; i < n_age * n_time; i++)
    {   if( count[i] != 1 )
        {   size_t j_time = i % n_time;
            size_t j_age  = (i - j_time) / n_time;
            cerr << "weight_grid table with weight_id = " << weight_id
            << endl << "age_id = " << age_id_[j_age]
            << ", time_id = " << time_id_[j_time] << " appears "
            << count[i] << " times (not 1 time)." << endl;
        }
    }
}

// weight_grid_group
void weight_grid_group(
    size_t                                   n_weight          ,
    const CppAD::vector<weight_grid_struct>& weight_grid_table ,
    CppAD::vector<size_t>&                   grid_start        ,
    CppAD::vector<size_t>&                   grid_order        )
{   //
    // n_grid
    size_t n_grid = weight_grid_table.size();
    //
    // grid_start
    // set grid_start[weight_id + 1] to the number of rows for weight_id
    grid_start.resize(0);
    grid_start.resize(n_weight + 1);
    for(size_t weight_id = 0; weight_id <= n_weight; weight_id++)
        grid_start[weight_id] = 0;
    size_t n_order = 0;
    for(size_t i = 0; i < n_grid; i++)
    {   int id = weight_grid_table[i].weight_id;
        if( 0 <= id && size_t(id) < n_weight )
        {   grid_start[size_t(id) + 1]++;
            n_order++;
        }
    }
    //
    // grid_start
    // set grid_start[weight_id] to the index in grid_order of the first row
    // for weight_id; grid_start[n_weight] is the size of grid_order.
    for(size_t weight_id = 0; weight_id < n_weight; weight_id++)
        grid_start[weight_id + 1] += grid_start[weight_id];
    assert( grid_start[n_weight] == n_order );
    //
    // next
    // next[weight_id] is the index in grid_order for next row for weight_id
    CppAD::vector<size_t> next(n_weight);
    for(size_t weight_id = 0; weight_id < n_weight; weight_id++)
        next[weight_id] = grid_start[weight_id];
    //
    // grid_order
    grid_order.resize(0);
    grid_order.resize(n_order);
    for(size_t i = 0; i < n_grid; i++)
    {   int id = weight_grid_table[i].weight_id;
        if( 0 <= id && size_t(id) < n_weight )
            grid_order[ next[size_t(id)]++ ] = i;
    }
}

} // END_DISMOD_AT_NAMESPACE
