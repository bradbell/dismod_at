# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
'''
{xrst_begin plot_data_fit}
{xrst_spell
   pdf
}

Plot The Data Fit By Integrand
##############################

Syntax
******
{xrst_literal
   # BEGIN syntax
   # END syntax
}

database
********
This ``str`` is the file name for
an existing dismod_at database that contains the results of a
:ref:`fit_command-name` .
This argument can't be ``None`` .

pdf_file
********
This ``str``
is the location where the pdf file containing the plot will be placed.
This argument can't be ``None`` .

plot_title
**********
This ``str`` is extra text printed at the beginning of the
title for each plot.
If this argument is ``None`` , there is no extra text.

max_plot
********
This is a ``int`` specifying the maximum number of points
to plot per integrand.
If this argument is less that the number of values for an integrand,
the values are randomly sub-sampled keeping the same order.
The x-axis (data index) is the original index value before sub-sampling.
If this argument is ``None`` no sub-sampling is done.

integrand_list
**************
Each element of this ``list`` is a ``str`` containing an
:ref:`integrand_table@integrand_name`
that we are plotting the fit for.
If this argument is ``None`` all of the integrands,
except those with names that begin with ``mulcov_`` ,
are included.

n_fit_dict
**********
This is a dictionary with keys that are the integrand names
in *integrand_list* .
(If *integrand_list* is ``None`` , the keys are all the
integrand names that do not begin with ``mulcov_`` .)
The value *n_fit_dict* [ *key* ] is the number of rows
in the :ref:`data_subset_table-name` that correspond to this integrand and
are not held out in the data table (see below).
If this is less than 2, the corresponding integrand is not plotted.

Plot Legend
***********

Data Table Hold Out
===================
We use data table hold out to refer to values that have
*hold_out* equal to one in the data table.
This does not include hold outs created by the hold out command
or hold outs created by the hold option.
The data table hold out values are not included during the automatic
choice of the plotting limits.

Point Symbol
============
Values that are within the automatically chosen plotting limits
are plotted using the point character ``.`` .

Plus Symbol
===========
Values that are outside the automatically chosen plotting limits
are plotted using the point plus character ``+`` .

Green
=====
Values that correspond to data that is held out in the data table
are plotted using the color green.

Red and Black
=============
Values that correspond to data that is not held out in the data table,
and is not within (is within) the automatically chosen limits,
are plotted using the color red (black).

Example
*******
:ref:`user_plot_data_fit.py-name` .

{xrst_end plot_data_fit}
'''
# ----------------------------------------------------------------------------
import random
import numpy
import dismod_at
from matplotlib import pyplot
import matplotlib.backends.backend_pdf
# ----------------------------------------------------------------------------
def plot_data_fit(
# BEGIN syntax
# n_fit_dict = plot_data_fit(
   database          = None,
   pdf_file          = None,
   plot_title        = None,
   max_plot          = None,
   integrand_list    = None,
# )
# END syntax
) :
   assert not database is None
   assert not pdf_file is None
   #
   # tables
   new        = False
   connection = dismod_at.create_connection(database, new)
   tables     = dict()
   for name in [
      'data',
      'data_subset',
      'integrand',
      'fit_data_subset',
   ] :
      tables[name] = dismod_at.get_table_dict(connection, name)
   connection.close()
   #
   # pdf
   pdf = matplotlib.backends.backend_pdf.PdfPages(pdf_file)
   #
   # integrand_list
   if integrand_list is None :
      integrand_list = list()
      for row in tables['integrand'] :
         integrand_name = row['integrand_name']
         if not integrand_name.startswith('mulcov_') :
            integrand_list.append( integrand_name )
   #
   n_fit_dict = dict()
   for integrand_name in integrand_list :
      #
      # integrand_id
      integrand_id = None
      for (row_id, row) in enumerate(tables['integrand']) :
         if row['integrand_name'] == integrand_name :
            integrand_id = row_id
      if integrand_id is None :
         msg = f'Cannot find {integrand_name} in integrand table for '
         msg += database
         assert False, msg
      #
      # info_list
      info_list = list()
      #
      # subset_id, subset_row
      for (subset_id, subset_row) in enumerate(tables['data_subset']) :
         #
         # data_row
         data_id  = subset_row['data_id']
         data_row = tables['data'][data_id]
         #
         if data_row['integrand_id'] == integrand_id :
            #
            # meas_value
            meas_value = data_row['meas_value']
            #
            # hold_out
            hold_out = data_row['hold_out']
            #
            # age
            age = (data_row['age_lower'] + data_row['age_upper']) / 2.0
            #
            # time
            time = (data_row['time_lower'] + data_row['time_upper']) / 2.0
            #
            # avg_integreand, weighted_residual
            row               = tables['fit_data_subset'][subset_id]
            avg_integrand     = row['avg_integrand']
            weighted_residual = row['weighted_residual']
            #
            # info_list
            info = {
               'meas_value'        : meas_value ,
               'model'             : avg_integrand ,
               'residual'          : weighted_residual ,
               'hold_out'          : hold_out ,
               'index'             : len(info_list),
               'age'               : age,
               'time'              : time,
            }
            info_list.append( info )
      #
      # n_point
      n_point = len( info_list )
      #
      if n_point == 0 :
         #
         # n_hold_out
         n_hold_out = 0
      else :
         # numpy_info
         keys       = info_list[0].keys()
         numpy_info = dict()
         for key in keys :
            vector = numpy.zeros(n_point, dtype=float)
            for i in range( n_point ) :
               vector[i] = info_list[i][key]
            numpy_info[key] = vector
         #
         # hold_out, not_hold_out, n_hold_out
         hold_out     = (numpy_info['hold_out'] == 1)
         not_hold_out = numpy.logical_not(hold_out)
         n_hold_out   = sum( hold_out )
      #
      # n_fit_dict
      n_fit_dict[integrand_name] = n_point - n_hold_out
      #
      if n_point - n_hold_out > 1 :
         #
         #
         # y_min, y_max
         d_fit       = numpy_info['meas_value'][not_hold_out]
         d_median    = numpy.median( d_fit )
         d_max       = d_median * 1e+3
         d_min       = d_median * 1e-3
         assert d_min >= 0.0
         #
         # r_min, r_max
         r_fit       = numpy_info['residual'][not_hold_out]
         r_norm      = numpy.linalg.norm( r_fit )
         r_avg_sq    = r_norm * r_norm / (n_point - n_hold_out)
         r_max       = 4.0 * numpy.sqrt( r_avg_sq )
         r_min       = - r_max
         #
         # subplot_list
         subplot_list = [ 'meas_value', 'model', 'residual' ]
         #
         # numpy_info
         for name in [ 'meas_value', 'model' ] :
            numpy_info[name] = numpy.maximum( numpy_info[name], d_min )
            numpy_info[name] = numpy.minimum( numpy_info[name], d_max )
         for name in [ 'residual' ] :
            numpy_info[name] = numpy.maximum( numpy_info[name], r_min )
            numpy_info[name] = numpy.minimum( numpy_info[name], r_max )
         #
         if max_plot is None or  n_point <= max_plot :
            #
            # n_plot
            n_plot = n_point
         else :
            #
            # n_plot
            n_plot = max_plot
            #
            # subsample
            subsample = random.sample( range(n_point), max_plot )
            subsample = sorted(subsample)
            #
            # numpy_info
            for key in numpy_info :
               numpy_info[key] = numpy_info[key][subsample]
            #
            # hold_out, not_hold_out
            hold_out     = (numpy_info['hold_out'] == 1)
            not_hold_out = numpy.logical_not(hold_out)
            #
         #
         # point_size, marker_size
         point_size  = numpy.array( n_plot * [ 1 ] )
         marker_size = numpy.array( n_plot * [ 10 ] )
         #
         for x_name in [ 'index', 'age', 'time' ] :
            #
            # subplot setup
            fig, axes = pyplot.subplots(3, 1, sharex=True)
            fig.subplots_adjust(hspace=0)
            #
            # x
            x = numpy_info[x_name]
            #
            for subplot_index in range(3) :
               # sp
               sp   = pyplot.subplot(3, 1, subplot_index + 1)
               #
               # name, y
               name = subplot_list[subplot_index]
               y    =  numpy_info[name]
               #
               # ylabel
               pyplot.ylabel(name)
               #
               # clip_list, limit_list
               if name == 'residual' :
                  clip_list  = [ r_min, r_max ]
                  limit_list = [ 1.1 * r_min, 1.1 * r_max ]
               else :
                  pyplot.yscale('log')
                  clip_list  = [ d_min, d_max ]
                  limit_list = [ 0.9 * d_min, 1.1 * d_max ]
               #
               # ylim
               pyplot.ylim(limit_list[0], limit_list[1])
               #
               # clipped, not_clipped
               clipped = (y == clip_list[0])
               clipped = numpy.logical_or(clipped, (y == clip_list[1]) )
               not_clipped  = numpy.logical_not(clipped)
               #
               green_point  = numpy.logical_and(hold_out, not_clipped)
               green_marker = numpy.logical_and(hold_out, clipped)
               black_point  = numpy.logical_and(not_hold_out, not_clipped)
               red_marker   = numpy.logical_and(not_hold_out, clipped)
               #
               # plot green points
               size = point_size[green_point]
               pyplot.scatter(x[green_point], y[green_point],
                  marker='.', color='green', s=size
               )
               #
               # plot green marker
               size = marker_size[green_marker]
               pyplot.scatter(x[green_marker], y[green_marker],
                  marker='+', color='green', s=size
               )
               #
               # plot black points
               size = point_size[black_point]
               pyplot.scatter(x[black_point], y[black_point],
                  marker='.', color='black', s=size
               )
               #
               # plot red marker
               size = marker_size[red_marker]
               pyplot.scatter(x[red_marker], y[red_marker],
                  marker='+', color='red', s=size
               )
               if name == 'residual' :
                  y = 0.0
                  pyplot.axhline(
                     y, linestyle='solid', color='black', alpha=0.3
                  )
               if subplot_index == 0 :
                  if plot_title is None :
                     pyplot.title( integrand_name )
                  else :
                     pyplot.title( plot_title + ': ' + integrand_name )
            # x-axis label
            pyplot.xlabel(x_name)
            #
            # save plot
            pdf.savefig( fig )
            pyplot.close( fig )
   # end of pages in pdf file
   pdf.close()
   return n_fit_dict
