# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_covid_19.py}
# {xrst_spell
#     covid
#     dt
#     rcll
# }
# {xrst_comment_ch #}
#
# Model The Covid-19 Epidemic
# ###########################
#
# Goal
# ****
# Use previous cumulative death data for a location to predict future
# cumulative deaths.
#
# Data File
# *********
# The cumulative death data is the most reliable data available for the
# Covid-19 epidemic. It is also the statistic we are most interested in predicting.
# Our example data has the following columns:
#
# .. list-table::
#     :widths: auto
#
#     * - *day*
#       - day that this row of data corresponds to
#     * - *death*
#       - cumulative covid_19 death as fraction of population
#     * - *mobility*
#       - mobility shifted and scaled to be in [-1, 0.]
#     * - *testing*
#       - testing shifted and scaled to be in [0., 1]
#
# We assume that difference in the cumulative death data are independent; i.e.,
# the amount added to the cumulative death is the actual measurement recorded
# at the end of a time interval.
# For the purpose of this example, we mimic a csv file with a string that
# has this information.
#
# SIR Model
# *********
#
# ODE
# ===
# We use :math:`Q` in place of :math:`S` in the SIR Model
# to avoid confusion with the dismod meaning of :math:`S`.
# The SIR Model for an epidemic is the following differential equations:
#
# .. math::
#
#  \begin{array}{rcll}
#   \dot{Q} & = & - \beta Q I \\
#   \dot{I} & = & + \beta Q I & - ( \gamma + \chi ) I \\
#   \dot{R} & = & + \gamma I
#  \end{array}
#
# .. csv-table::
#     :widths: auto
#
#     :math:`Q(t)`,susceptible fraction of the population
#     :math:`I(t)`,infectious fraction of the population
#     :math:`R(t)`,recovered fraction of the population
#     :math:`\beta(t)`,infectious rate
#     :math:`\gamma(t)`,recovery rate
#     :math:`\chi(t)`,excess mortality rate (for this disease)
#
# Data Model
# ==========
# The model for a measurement of the i-th difference in cumulative death is
#
# .. math::
#
#  d_i = e_i + \int_{a(i)}^{b(i)} \chi(t) I (t) dt
#
# .. csv-table::
#     :widths: auto
#
#     :math:`d_i`,the i-th difference in cumulative death
#     :math:`e_i`,noise in the i-th difference in cumulative death
#     :math:`a_i`,start time for i-th difference in cumulative death
#     :math:`b_i`,end time for i-th difference in cumulative death
#
# Discussion
# ==========
# The model does not include births and deaths due to other causes, but this
# would introduce even more unknowns.
# Another problem with the model above is that it is not identifiable given just
# measurements of cumulative death; i.e., there are two many unknown functions.
# One approach to this problem is to assume we know
# the rates :math:`\gamma(t)`, :math:`\chi(t)` and
# only estimate :math:`\beta(t)` and the initial conditions
# :math:`Q(0)`, :math:`I(0)`, :math:`R(0)`.
# Another problem is that deaths are often under reported; e.g., they might only
# be the deaths in the hospitals or for people who have been confirmed to have the
# disease.
#
# Dismod Model
# ************
# The Dismod model for an epidemic is the following ODE:
#
# .. math::
#
#  \begin{array}{rcll}
#   \dot{S} & = & - \iota S & + \rho            C    \\
#   \dot{C} & = & + \iota S & - ( \rho + \chi ) C
#  \end{array}
#
# together with the following cumulative death model
#
# .. math::
#
#  d_i = e_i + \int_{a(i)}^{b(i)} \chi(t) \frac{C(t)}{S(t) + C(t)} dt
#
# Remission :math:`\rho` is :math:`\gamma` in the SIR model.
# Prevalence :math:`C(t) / [ S(t) + C(t) ]`
# is to :math:`I(t)` in the SIR model.
# Susceptible :math:`S(t)` is :math:`Q(t) + R(t)` in the SIR model.
# Not all the members of S are susceptible to the disease; i.e., the members of R.
# It would be nice to complete the :ref:`wish_list@Immunity`
# wish list item so this would not be necessary.
# However, because of under reporting of deaths, :math:`Q(t)` in the SIR model
# also has members that are not susceptible to the disease.
#
# Data
# ****
# For this example, we get the cumulative death data and covariates from a
# text string version of a CSV file with the following columns:
#
# .. csv-table::
#     :widths: auto
#
#     *day*,days since the first cumulative death value
#     *death*,cumulative death value for this day
#     *mobility*,a social mobility covariate
#     *testing*,level of testing for the disease covariate
#
# The *mobility* covariate has been shifted and scaled to be in the
# interval [-1, 0] with zero corresponding to normal mobility.
# The *testing* covariate has been scaled to the interval [0, 1]
# with zero corresponding to no testing.
#
# Model Variables
# ***************
# There is only one location for this example, so there are no random effects.
# If you had data from multiple locations you could use random effects to improve
# the estimation.
#
# Rates
# =====
# The prior for the value of :math:`\iota(t)` is uniform with
# lower limit 1e-6, upper limit 1.0, and mean 1e-3.
# The mean is only used as a starting point and scaling point for the optimization.
# The prior for the forward difference of :math:`\iota(t)` between grid points is
# log Gaussian with mean zero, standard deviation 0.1 and the offset in the
# log transformation is 1e-5. The corresponds to a difference of 10 percent between
# days being a weighted residual of one.
# The other rates are modeled as the following known constants:
# :math:`\rho = 0.1` and :math:`\chi = 0.01`.
#
# Covariates
# ==========
# There are two covariate multipliers for this example, one for mobility
# and the other for testing. These affect the rate :math:`\iota(t)`.
# They are both have one grid point (are constant in time) and
# have a uniform prior with lower limit -1, upper limit +1, and mean zero.
# It appears that the bounds on the covariate multipliers are active at the
# solution; i.e., more effect would improve the objective; see the file
# :ref:`db2csv_command@variable.csv` generated by the
# ``db2csv`` command after the fit was done.
#
# Predictions
# ***********
# To do predictions for the next week, we could fit a function to the previous
# weeks baseline value of :math:`\iota(t)` (value without the covariates effects).
# This gives us a prediction for the baseline value of :math:`\iota(t)`
# for the next week. We would then include some assumed covariate values for the next
# and predict the differences in cumulative death for each day during the next week.
# The subtitle point here is that the prior for :math:`\iota(t)` is a constant,
# but the posterior is not. Actually :math:`\beta(t)` in the SIR model
# is more likely to be constant and it might be better to fit the :math:`\beta(t)`
# that corresponds to the previous fit for :math:`I(t) = C(t) / [ S(t) + C(t) ]`.
#
# Display Fit
# ***********
# If this variable is true, some of the fit results will be printed and plotted:
# {xrst_spell_off}
# {xrst_code py}
display_fit = True
# {xrst_code}
# {xrst_spell_on}
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_covid_19.py}
#
# $end
# -----------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import csv
import copy
import numpy
import matplotlib.pyplot
import matplotlib.gridspec
test_program = 'example/user/covid_19.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
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
# ------------------------------------------------------------------------------------
csv_file_data = '''day,death,mobility,testing
0,1.5159e-07,0,0.012738
1,4.0424e-07,-0.099623,0
2,8.59e-07,-0.20332,0.017105
3,1.2127e-06,-0.30759,0.06067
4,2.0717e-06,-0.40911,0.14986
5,3.2844e-06,-0.50501,0.21134
6,4.9014e-06,-0.59301,0.26117
7,6.9226e-06,-0.67147,0.34118
8,8.8932e-06,-0.73938,0.40973
9,1.1015e-05,-0.79633,0.4094
10,1.5917e-05,-0.84249,0.37192
11,2.1475e-05,-0.87867,0.38522
12,2.7842e-05,-0.90616,0.47688
13,3.724e-05,-0.92649,0.44716
14,5.3561e-05,-0.94127,0.42764
15,6.6042e-05,-0.95196,0.35542
16,8.4132e-05,-0.95975,0.44191
17,0.00010793,-0.96562,0.43481
18,0.00013663,-0.9703,0.47483
19,0.00016124,-0.97439,0.4991
20,0.00020601,-0.97832,0.57172
21,0.00025144,-0.98238,0.578
22,0.0002858,-0.98666,0.54818
23,0.00031389,-0.99101,0.51412
24,0.00036765,-0.99503,0.5705
25,0.00042076,-0.99821,0.64551
26,0.00047392,-1,0.71982
27,0.00051854,-0.99992,0.69938
28,0.00056846,-0.99765,0.64184
29,0.00061707,-0.993,0.54799
30,0.00066199,-0.98593,0.52809
31,0.00071307,-0.97664,0.57946
32,0.00076133,-0.9656,0.66222
33,0.00079933,-0.95358,0.68223
34,0.00083738,-0.94159,0.64932
35,0.00087133,-0.93087,0.615
36,0.00090342,-0.92271,0.54825
37,0.0009345,-0.9183,0.47447
38,0.00096795,-0.91827,0.47153
39,0.0009973,-0.9223,0.55527
40,0.0010307,-0.92956,0.73615
41,0.0010602,-0.93907,0.97671
42,0.0010865,-0.94973,1
43,0.0011096,-0.96031,0.83756
44,0.0011353,-0.9694,0.61103
45,0.0011592,-0.97554,0.60876
46,0.0011882,-0.97761,0.6771
47,0.0012051,-0.97497,0.73262
48,0.0012176,-0.96751,0.79038
49,0.0012257,-0.95556,0.80307
50,0.0012519,-0.93992,0.81576
51,0.0012667,-0.92161,0.82845
52,0.0012797,-0.90996,0.84115
53,0.0013138,-0.90255,0.85384
54,0.0013249,-0.89783,0.86653
'''
# ====================================================================================
def example_db(file_name) :
   def fun_iota_prior (a, t) :
      return ('prior_iota_value', None, 'prior_iota_dtime')
   def fun_pini_prior (a, t) :
      return ('prior_pini_value', None, None)
   def fun_rho_prior (a, t) :
      return ('prior_rho_value', None, None)
   def fun_chi_prior (a, t) :
      return ('prior_chi_value', None, None)
   def fun_covariate_prior (a, t) :
      return ('prior_covariate_value', None, None)
   #
   # file_data
   file_data  = list()
   reader    = csv.DictReader( csv_file_data.splitlines() )
   for row in reader :
      for key in row :
         row[key] = float( row[key] )
      file_data.append( copy.copy( row ) )
   #
   # age_list
   # This analysis has no age variation
   age_list = [0.0, 100.0]
   #
   # time_list
   time_list = list()
   for row in file_data :
      time_list.append( row['day'] )
   #
   # integrand_table
   integrand_table = [ { 'name' : 'mtspecific' } ]
   #
   # node_table
   node_table = [ { 'name': 'world', 'parent': '' } ]
   #
   # subgroup_table
   subgroup_table = [ {'subgroup' : 'world', 'group' : 'world' } ]
   #
   # weight_table
   weight_table = list()
   #
   # covariate_table
   covariate_table = [ {
      'name'      : 'mobility',
      'reference' : 0.0,
      },{
      'name'      : 'testing',
      'reference' : 0.0,
   } ]
   #
   # avgint_table
   avgint_table = list()
   #
   # data_table
   data_table = list()
   row_out = {
      'hold_out'  : False       ,
      'integrand' : 'mtspecific',
      'node'      : 'world'     ,
      'subgroup'  : 'world'     ,
      'weight'    : ''          ,
      'age_lower' : 50.0        ,
      'age_upper' : 50.0        ,
      'density'   : 'gaussian'  ,
   }
   data_cv = 0.25
   n_data  = len(file_data) - 1
   for i in range(n_data) :
      row_avg = dict()
      for key in [ 'day', 'mobility', 'testing' ] :
         row_avg[key] = ( file_data[i][key] + file_data[i+1][key] ) / 2.0
      row_out['time_lower'] = row_avg['day']
      row_out['time_upper'] = row_avg['day']
      row_out['mobility']   = row_avg['mobility']
      row_out['testing']    = row_avg['testing']
      death_difference      = file_data[i+1]['death'] - file_data[i]['death']
      day_difference        = file_data[i+1]['day'] - file_data[i]['day']
      row_out['meas_value'] = death_difference / day_difference
      row_out['meas_std']   = data_cv * row_out['meas_value']
      data_table.append( copy.copy(row_out) )
   #
   # prior_table
   prior_table = [ {
      'name'    : 'prior_iota_value',
      'lower'   : 1e-6,
      'upper'   : 1e0,
      'mean'    : 1e-3,
      'density' : 'uniform',
   },{
      'name'    : 'prior_iota_dtime',
      'density' : 'log_gaussian',
      'mean'    : 0.0,
      'std'     : 0.1,
      'eta'     : 1e-5,
   },{
      'name'    : 'prior_rho_value',
      'density' : 'uniform',
      'mean'    : 0.1,
      'lower'   : 0.1,
      'upper'   : 0.1,
   },{
      'name'    : 'prior_chi_value',
      'density' : 'uniform',
      'mean'    : 0.01,
      'lower'   : 0.01,
      'upper'   : 0.01,
   },{
      'name'    : 'prior_pini_value',
      'density' : 'uniform',
      'lower'   : 1e-7,
      'upper'   : 1e-3,
      'mean'    : 1e-5,
   },{
      'name'    : 'prior_covariate_value',
      'density' : 'uniform',
      'mean'    : 0.0,
      'lower'   : - 1.0,
      'upper'   : + 1.0,
   } ]
   #
   # smooth_table
   time_id = list( range( len( time_list ) ) )
   smooth_table = [ {
      'name'    : 'smooth_pini',
      'age_id'  : [ 0 ],
      'time_id' : [ 0 ],
      'fun'     : fun_pini_prior,
   },{
      'name'    : 'smooth_iota',
      'age_id'  : [ 0 ],
      'time_id' : time_id,
      'fun'     : fun_iota_prior,
   },{
      'name'    : 'smooth_rho',
      'name'    : 'smooth_rho',
      'age_id'  : [ 0 ],
      'time_id' : [ 0 ],
      'fun'     : fun_rho_prior,
   },{
      'name'    : 'smooth_chi',
      'age_id'  : [ 0 ],
      'time_id' : [ 0 ],
      'fun'     : fun_chi_prior,
   },{
      'name'    : 'smooth_covariate',
      'age_id'  : [ 0 ],
      'time_id' : [ 0 ],
      'fun'     : fun_covariate_prior,

   } ]
   #
   # nslist_dict
   nslist_dict = dict()
   #
   # rate_table
   rate_table = [ {
      'name'          : 'pini',
      'parent_smooth' : 'smooth_pini',
   },{
      'name'          : 'iota',
      'parent_smooth' : 'smooth_iota',
   },{
      'name'          : 'rho',
      'parent_smooth' : 'smooth_rho',
   },{
      'name'          : 'chi',
      'parent_smooth' : 'smooth_chi',
   } ]
   #
   # mulcov_table
   mulcov_table = [ {
      'covariate' : 'mobility',
      'type':       'rate_value',
      'effected':   'iota',
      'group':      'world',
      'smooth':     'smooth_covariate',
   },{
      'covariate' : 'testing',
      'type':       'rate_value',
      'effected':   'iota',
      'group':      'world',
      'smooth':     'smooth_covariate',
   } ]
   #
   # option_table
   if display_fit :
      print_level = '5'
   else :
      print_level = '0'
   option_table = [
      { 'name'  : 'parent_node_name',   'value' : 'world'            },
      { 'name'  : 'rate_case',          'value' : 'iota_pos_rho_pos' },
      { 'name'  : 'print_level_fixed',  'value' : print_level        },
      { 'name'  : 'quasi_fixed',        'value' : 'false'            },
      { 'name'  : 'tolerance_fixed',    'value' : '1e-11'            },
   ]
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name,
      age_list,
      time_list,
      integrand_table,
      node_table,
      subgroup_table,
      weight_table,
      covariate_table,
      avgint_table,
      data_table,
      prior_table,
      smooth_table,
      nslist_dict,
      rate_table,
      mulcov_table,
      option_table
   )
   return
# ===========================================================================
# create the database
file_name  = 'example.db'
example_db(file_name)
#
# fit data
program_cpp = '../../devel/dismod_at'
program_py  = '../../../python/bin/dismodat.py'
dismod_at.system_command_prc([ program_cpp, file_name, 'init' ])
dismod_at.system_command_prc([ program_cpp, file_name, 'fit', 'fixed' ])
dismod_at.system_command_prc([ program_py,  file_name, 'db2csv' ])
#
# weighted residuals
file_in  = 'data.csv'
file_in  = open(file_in, 'r')
reader   = csv.DictReader(file_in)
time_r   = list()
residual = list()
for row in reader :
   assert row['integrand'] == 'mtspecific'
   assert row['time_lo']   == row['time_up']
   time_r.append( float( row['time_lo'] ) )
   residual.append( float( row['residual'] ) )
residual_max  = numpy.max(residual)
residual_min  = numpy.min(residual)
residual_avg  = numpy.mean(residual)
#
if display_fit :
   # print residual statistics
   print( 'residual_max = ', residual_max )
   print( 'residual_max = ', residual_max )
   print( 'residual_avg = ', residual_avg )
   #
   # plot setup
   fig     = matplotlib.pyplot.figure(tight_layout = True)
   gs      = matplotlib.gridspec.GridSpec(2,1)
   ax1     = fig.add_subplot(gs[0,0])
   ax2     = fig.add_subplot(gs[1,0])
   #
   # plot death data weighted residuals
   ax2.plot(time_r, residual, 'k+')
   ax2.plot( [time_r[0], time_r[-1]], [0.0, 0.0], 'k-')
   ax2.set_xlabel('time')
   ax2.set_ylabel('weighted residuals')
   #
   # plot iota(t)
   file_in = 'variable.csv'
   file_in = open(file_in, 'r')
   reader  = csv.DictReader(file_in)
   time_i  = list()
   iota    = list()
   for row in reader :
      if row['var_type'] == 'rate' and row['rate'] == 'iota' :
         time_i.append( float(row['time']) )
         iota.append( float(row['fit_value']) )
   ax1.plot(time_i, iota, 'k-')
   ax1.set(xlabel = 'day')
   ax1.set(ylabel = 'iota' )
   file_in.close()
   #
   # display plot
   matplotlib.pyplot.show()
#
# check residuals
assert residual_max < 4.0
assert -4.0 < residual_min
assert abs( residual_avg ) < 0.5
#
print('covid_19.py: OK')
sys.exit(0)
# END PYTHON
