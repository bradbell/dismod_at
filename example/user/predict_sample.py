# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
r'''
{xrst_begin user_predict_sample.py}
{xrst_spell
}

Predict Using Posterior Samples
###############################

Purpose
*******
The command

| |tab|   ``dismod_at`` *database* ``predict sample fit_var`` *scale*

computes a prediction for each avgint table :ref:`avgint_table@avgint_id`
and each sample table :ref:`sample_table@sample_index` .

Note Table
**********
This example has only one node called ``world``

Model Variables
***************
There is only one model variable *iota* in this example and it is
constant w.r.t; age and time.


Measurement
***********
The data are measurements of
:ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence` ; i.e.
a direct measurement of *iota* .
We use *n_data* for the number of measurements.
The data is simulated using a normal with mean equal to the true *iota*
and standard deviation equal to *std_data* .
This distribution is also used for the model of the measurement noise.
{xrst_code py}'''
iota_true = 0.01
n_data    = 100
std_data  = iota_true * 0.2
r'''{xrst_code}

Random Seed
***********
This is used to seed the random number generator used to simulate the noise
in the measurement values:
{xrst_code py}'''
import time
random_seed = int( time.time() )
r'''{xrst_code}

Number of Samples
*****************
Each sample has a complete set of the :ref:`model_variables-name` which
yields a different prediction :`
{xrst_code py}'''
number_samples = 100
r'''{xrst_code}

Sample Covariance
*****************
Let :math:`\iota` be *iota* ,
math:`y_i` be the i-th simulated measurement,
:math:`n` be the number of data points *n_data* ,
and :math:`\sigam` bi the measurement standard deviation *std_data* .
Dropping terms that do not depend on *iota* , the negative log-likelihood is

.. math::
   L( \iota ) = \frac{1}{2} \sum_{i=0}^{m-1}
      \left( \frac{y_i - \iota }{\sigma} \right)^2

Taking the second derivative w.r.t. *iota* is :math:`m / \sigma^2` .
Hence the samples for *iota* should have variance ;math:`\sigma^2 / m` .

Prediction Covariance
*********************
The samples are scaled by *scale* before making predictions:
{xrst_code py}'''
scale = 2.0
r'''{xrst_code}
This should make the variance for the predictions should be

   (scale * sigma)^2 / n_data


Source Code
***********
The source code below does not include the global settings defined above:
{xrst_literal
   # BEGIN SOURCE
   # END SOURCE
}

{xrst_end user_predict_sample.py}
-------------------------------------------------------------------------------
'''
# BEGIN SOURCE
#
# imports
import sys
import os
import copy
#
import random
random.seed(random_seed)
#
# test_program
test_program  = 'example/user/predict_sample.py'
check_program = sys.argv[0].replace('\\', '/')
if check_program != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
#
# -----------------------------------------------------------------------------
# example_db
def example_db(file_name) :
   #
   # age_list, time_list
   age_list = [ 0.0,     100.0  ]
   time_list = [ 1980.0, 2020.0 ]
   #
   # integrand_table
   integrand_table = [
      { 'name' : 'Sincidence' } ,
      { 'name' : 'prevalence' } ,
   ]
   #
   # node_table
   node_table = [
      { 'name' : 'world' , 'parent' : '' } ,
   ]
   #
   # subgroup_table
   subgroup_table = [
      { 'subgroup' : 'world' , 'group' : 'world' } ,
   ]
   #
   # avgint_table
   avgint_table = [
      {
         'integrand'    : 'Sincidence',
         'node'         : 'world',
         'subgroup'     : 'world',
         'weight'       : '',
         'age_lower'    : 50.0,
         'age_upper'    : 50.0,
         'time_lower'   : 2000.0,
         'time_upper'   : 2000.0,
      }
   ]
   #
   # data_table
   data_table = list()
   row                = copy.copy( avgint_table[0] )
   row['hold_out']    = False
   row['density']     = 'gaussian'
   row['meas_std']    = std_data
   for i in range(n_data) :
      row['meas_value'] = random.normalvariate( iota_true, std_data )
      data_table.append( copy.copy( row ) )
   #
   # prior_table
   prior_table = [
      # parent_iota_prior
      {  'name'      : 'parent_iota_prior' ,
         'density'   : 'uniform' ,
         'lower'     : 1e-2 * iota_true ,
         'upper'     : 1e+2 * iota_true ,
         'mean'      : iota_true / 3.0  ,
      }
   ]
   #
   # smooth_table
   parent_iota_fun = lambda a,t : ('parent_iota_prior', None, None)
   smooth_table = [
      # parent_iota_smooth
      {  'name'      : 'parent_iota_smooth' ,
         'age_id'    : [ 0 ],
         'time_id'   : [ 0 ],
         'fun'       : parent_iota_fun,
      }
   ]
   #
   # rate_table
   rate_table = [
      {  'name'            : 'iota',
         'parent_smooth'   : 'parent_iota_smooth',
         'child_smooth'    : None,
      }
   ]
   #
   # option_table
   option_table = [
      {  'name' : 'parent_node_name',     'value' : 'world'             },
      {  'name' : 'random_seed',          'value' : str(random_seed)    },
      {  'name' : 'rate_case',            'value' : 'iota_pos_rho_zero' },
   ]
   #
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name          = file_name,
      age_list           = age_list,
      time_list          = time_list,
      integrand_table    = integrand_table,
      node_table         = node_table,
      subgroup_table     = subgroup_table,
      weight_table       = list(),
      covariate_table    = list(),
      avgint_table       = avgint_table,
      data_table         = data_table,
      prior_table        = prior_table,
      smooth_table       = smooth_table,
      nslist_dict        = dict(),
      rate_table         = rate_table,
      mulcov_table       = list(),
      option_table       = option_table,
   )
# -----------------------------------------------------------------------------
# main
def  main() :
   #
   # program
   program = '../../devel/dismod_at'
   #
   # example.db
   file_name = 'example.db'
   example_db(file_name)
   dismod_at.system_command_prc( [ program, file_name, 'init'] )
   #
   # example.db: fit_var table
   dismod_at.system_command_prc( [ program, file_name, 'fit', 'fixed'] )
   #
   # example.db: sample table
   ns = str( number_samples )
   dismod_at.system_command_prc(
      [ program, file_name, 'sample', 'asymptotic', 'fixed', ns ] )
   #
   # example.db: predict table
   dismod_at.system_command_prc(
      [ program, file_name, 'predict', 'sample', 'fit_var', str(scale) ] )
   #
   # fit_var_table, predict_table
   connection    = dismod_at.create_connection(
      file_name, new = False, readonly = True
   )
   fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
   predict_table = dismod_at.get_table_dict(connection, 'predict')
   assert len(fit_var_table) == 1
   assert len( predict_table) == number_samples
   connection.close()
   #
   # sample_variance
   sumsq         = 0.0
   fit_var_value = fit_var_table[0]['fit_var_value']
   for row in predict_table :
      sample = row['avg_integrand']
      sumsq += (sample - fit_var_value) * (sample - fit_var_value)
   sample_variance = sumsq / number_samples
   #
   # check
   theory_variance = (scale * std_data)**2  / n_data
   relerr          = (sample_variance - theory_variance) / theory_variance
   if abs(relerr) > 0.2 :
      print( f'random_seed = {random_seed}' )
      assert False, f'relerr = {relerr}'
#
main()
print('predict_sample.py: OK')
# END SOURCE
