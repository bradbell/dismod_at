// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin child_data_in_fit$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section For Each Child, Number of Data Points Included In Fit$$

$head Syntax$$
$icode%n_data_in_fit% = child_data_in_fit(
   %option_map%,
   %data_subset_table%,
   %integrand_table%,
   %data_table%,
   %child_info4data%
)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head option_map$$
The key $code hold_out_integrand$$ is present in $icode option_map$$ and
is the $cref/hold_out_integrand/option_table/hold_out_integrand/$$.
If it is not present, the empty string is used.

$head data_subset_table$$
is the $cref/data_subset_table/get_data_subset/data_subset_table/$$.

$head data_table$$
is the $cref/data_table/get_data_table/data_table/$$.

$head child_info4data$$
is a $cref child_info$$ object created using the data table.

$head n_data_in_fit$$
This vector has size equal to the number of children; i.e.
$cref/child_size/pack_info_sizes/child_size/$$.
The $th i$$ element of this vector is the number of data point
included in the fit
(not $cref/held out/fit_command/hold_out/$$).

$childtable%example/devel/utility/child_data_in_fit_xam.cpp
%$$
$head Example$$
The file $cref child_data_in_fit_xam.cpp$$ contains
and example and test of $code child_data_in_fit$$.
It returns true for success and false for failure.

$end
*/
# include <map>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_str_map.hpp>
# include <dismod_at/split_space.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<size_t> child_data_in_fit(
   const std::map<std::string, std::string>&    option_map            ,
   const CppAD::vector<data_subset_struct>&     data_subset_table     ,
   const CppAD::vector<integrand_struct>&       integrand_table       ,
   const CppAD::vector<data_struct>&            data_table            ,
   const child_info&                            child_info4data       )
// END_PROTOTYPE
{
   // sizes of const tables
   size_t n_subset    = data_subset_table.size();
   size_t n_integrand = integrand_table.size();
   size_t n_child     = child_info4data.child_size();
   //
   // initialize the result
   CppAD::vector<size_t> result(n_child);
   for(size_t child = 0; child < n_child; ++child)
      result[child] = 0;
   //
   // hold_out_integrand_list
   const std::string& hold_out_integrand = get_str_map(
      option_map, "hold_out_integrand", ""
   );
   CppAD::vector<std::string> hold_out_list = split_space(hold_out_integrand);
   //
   // hold_out_vec
   CppAD::vector<bool> hold_out_vec;
   size_t n_hold_out = hold_out_list.size();
   if( n_hold_out > 0 )
   {  hold_out_vec.resize(n_integrand);
      for(size_t integrand_id = 0; integrand_id < n_integrand; ++integrand_id)
      {  integrand_enum integrand = integrand_table[integrand_id].integrand;
         std::string    name      = integrand_enum2name[integrand];
         hold_out_vec[integrand_id] = false;
         for(size_t j = 0; j < n_hold_out; ++j)
            hold_out_vec[integrand_id] |= name == hold_out_list[j];
      }
   }
   //
   for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
   {  size_t data_id = size_t( data_subset_table[subset_id].data_id );
      //
      // integrand_id
      size_t integrand_id = size_t( data_table[data_id].integrand_id );
      //
      // hold_out
      bool hold_out = data_table[data_id].hold_out == 1;
      hold_out     |= data_subset_table[subset_id].hold_out == 1;
      if( hold_out_vec.size() != 0 )
         hold_out |= hold_out_vec[integrand_id];
      //
      if( ! hold_out )
      {
         //
         // child
         size_t child = child_info4data.table_id2child(data_id);
         //
         // update result for this child
         if( child < n_child )
            ++result[child];
      }
   }
   return result;
}

} // END DISMOD_AT_NAMESPACE
