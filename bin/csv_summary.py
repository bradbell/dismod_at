#! /bin/python3
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
#
import csv
import sys
#
# directory
if len(sys.argv) != 2 :
    usage  = 'bin/csv_summary.py directory\n'
    usage += 'where directory contains the db2csv output files'
    sys.exit(usage)
directory = sys.argv[1]
#
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
# file_list
file_list = [
    'age_avg',
    'data',
    'hes_fixed',
    'hes_random',
    'log',
    'option',
    'predict',
    'variable',
]
#
# file_data
table_dict = dict()
for name in file_list :
    table_dict[name] = read_file(name + '.csv')
#
#
table = table_dict['data']
(avg, count) = average(table, 'integrand', 'residual')
print_variable( 'avg_integrand_residual', avg )
print_variable( 'count_integrand_residual', count )
print()
#
table = table_dict['data']
(avg, count) = average(table, 'integrand', 'x_0')
print_variable( 'avg_integrand_x_0', avg )
print_variable( 'count_integrand_x_0', count )
print()
#
table = table_dict['variable']
table = subsample(table, 'var_type', 'rate')
(avg, count) = average(table, 'rate', 'res_value')
print_variable( 'avg_rate_residual', avg )
print_variable( 'count_rate_residual', count )
print()
