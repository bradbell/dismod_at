#! /bin/python3
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
#
import csv
import sys
#
if sys.argv[0] != 'bin/csv_summary.py' :
   msg = 'bin/csv_simmary.py must be executed from its parent directory'
#
if len(sys.argv) != 2 :
   usage  = 'bin/csv_summary.py directory\n'
   usage += 'where directory contains the db2csv output files'
   sys.exit(usage)
   sys.exit(msg)
#
# directory
directory = sys.argv[1]
# ---------------------------------------------------------------------------
# print_variable
def print_variable(name, value) :
   print( '{:25} = {}'.format( name , value) )
#
# read_file
def read_file(file_name) :
   file_path = directory + '/' + file_name
   file_ptr  = open(file_path)
   reader    = csv.DictReader(file_ptr)
   table     = list()
   for row in reader :
      table.append(row)
   return table
#
# avgerage
def average(table, variable_column, value_column) :
   avg  = dict()
   count = dict()
   for row in table :
      variable = row[variable_column]
      value    = row[value_column]
      if value != '' :
         if variable not in avg :
            avg[variable]   = 0.0
            count[variable] = 0
         avg[variable]   += float( value )
         count[variable] += 1
   for key in avg :
      avg[key]   = avg[key] / count[key]
   return (avg, count)
#
# subsample
def subsample(table, column_name, column_value) :
   result = list()
   for row in table :
      if row[column_name] == column_value :
         result.append(row)
   return result
#
# weighted average of weighted residuals
def weighted_average_residual(data_table) :
   avg_res    = dict()
   sum_weight = dict()
   count      = dict()
   for row in data_table :
      integrand = row['integrand']
      residual  = row['residual']
      density   = row['density']
      if residual != '' :
         sigma = row['meas_sigma']
         #
         # delta
         delta  = sigma
         is_log = density.startswith('log_') or density.startswith('cen_log_')
         if( is_log )
            meas_value = row['meas_value']
            eta        = row['eta']
            delta      = log(meas_value + eta + sigma) - log(meas_value + eta)
         #
         weight = 1.0 / float( delta )
         if integrand not in avg_res :
            avg_res[integrand]    = 0.0
            sum_weight[integrand] = 0.0
            count[integrand]      = 0
         avg_res[integrand]    += float( residual ) * weight
         sum_weight[integrand] += weight
         count[integrand]      += 1
   for key in avg_res :
      avg_res[key] = avg_res[key] / sum_weight[key]
   return(avg_res, count)
#
# db2csv output files that are used so fare (without .csv extension)
file_list = [
   'data',
   'variable',
]
#
# file_data
table_dict = dict()
for name in file_list :
   table_dict[name] = read_file(name + '.csv')
#
# covariate values
table = table_dict['data']
for key in table[0] :
   if key.startswith('x_') :
      (avg, count) = average(table, 'integrand', key)
      print_variable( 'avg_integrand_' + key, avg )
      print_variable( 'count_integrand_' + key, count )
      print()
# --------------------------------------------------------------------------
# integrand_meas_value, integrand_avgint, integrand_residual
table = table_dict['data']
(avg, count) = average(table, 'integrand', 'meas_value')
print_variable( 'avg_integrand_meas_value', avg )
count_integrand_meas_value = count
#
(avg, count) = average(table, 'integrand', 'avgint')
print_variable( 'avg_integrand_avgint', avg )
assert count_integrand_meas_value == count
#
(avg, count) = average(table, 'integrand', 'residual')
print_variable( 'avg_integrand_residual', avg )
assert count_integrand_meas_value == count
#
(avg, count) = weighted_average_residual(table)
print_variable( 'wavg_integrand_residual' , avg )
#
print_variable( 'count', count )
print()
# --------------------------------------------------------------------------
# rate_fit_valeu, rate_mean_value, rate_residual
table = table_dict['variable']
table = subsample(table, 'var_type', 'rate')
(avg, count) = average(table, 'rate', 'fit_value')
print_variable( 'avg_rate_fit_value', avg )
count_rate_file_value = count
#
(avg, count) = average(table, 'rate', 'mean_v')
print_variable( 'avg_rate_mean_value', avg )
assert count_rate_file_value == count
print_variable( 'count', count )
print()
#
(avg, count) = average(table, 'rate', 'res_value')
print_variable( 'avg_rate_residual', avg )
print_variable( 'count', count )
print()
# --------------------------------------------------------------------------
print('csv_summary.py: OK')
sys.exit(0)
