# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
'''
$begin plot_curve$$
$spell
   pdf
   std
   str
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Plot Log-Scaled Values With Respect To Age and Time$$

$head Syntax$$
$srcthisfile%0%# BEGIN syntax%# END syntax%1%$$

$head pdf_file$$
This argument can't be $code None$$.
It is a $code str$$ containing the location where the
pdf file containing the plot will be placed.

$head plot_title$$
This argument can be $code None$$.
It is a $code str$$ title printed at the top of every plot.

$head plot_limit$$
This argument can't be $code None$$.
It is a $code dict$$ with the following keys:

$subhead age_min$$
is a $code float$$ containing the minimum age in the plot region.

$subhead age_max$$
is a $code float$$ containing the maximum age in the plot region.

$subhead time_min$$
is a $code float$$ containing the minimum time in the plot region.

$subhead time_max$$
is a $code float$$ containing the maximum time in the plot region.

$head plot_data$$
This argument can't be $code None$$.
It is a $code dict$$ where each element is a $code list$$.
The key for the dictionary is the name of the value $icode z$$ being plotted.
For each $icode z_name$$ is in $icode plot_data$$,
$codei%
   %z_list% = %plot_data%[%z_name%]
%$$
is a $code list$$.
Each element of this list is a $code dict$$ with the following keys:
$code 'age'$$, $code 'time'$$, $code 'value'$$.
In addition, it may also contain the key $code 'std'$$.

$subhead age$$
For each $icode row$$ in $icode z_list$$, $icode%row%['age']%$$
is the age corresponding to this value of $icode z$$.

$subhead time$$
For each $icode row$$ in $icode z_list$$, $icode%row%['time']%$$
is the time corresponding to this value of $icode z$$.
For each age value in a $icode z_list$$,
the set of time value must be the same; i.e.,
the set of age an time values form a rectangular grid.

$subhead value$$
For each $icode row$$ in $icode z_list$$, $icode%row%['value']%$$
is the value of $icode z$$.
All of these values must be non-negative and they must
not all be zero.

$subhead std$$
For each $icode row$$ in $icode z_list$$, $icode%row%['std']%$$
is the standard deviation corresponding to this value of $icode z$$.
This key is optional. It appears for one element of a $icode z_list$$,
it must appear for all the elements of the $icode z_list$$.
This can change between different $icode z_name$$ values.
Furthermore, it this appears, it will be plotted and it must not be all zero
(for one $icode z_list$$).

$head Example$$
See $cref user_plot_curve.py$$

$end
'''
# ----------------------------------------------------------------------------
import numpy
import math
from matplotlib import pyplot
import matplotlib.backends.backend_pdf
# ----------------------------------------------------------------------------
# new_figure
# Matplotlib cannot handle multiple figures, so we re-use the same figure.
global_figure = None
def new_figure() :
   global global_figure
   if global_figure is None :
      global_figure= pyplot.figure()
   global_figure.clf()
   return global_figure
# ----------------------------------------------------------------------------
def plot_curve(
# BEGIN syntax
# plot_curve(
   pdf_file    = None ,
   plot_title  = None ,
   plot_limit  = None ,
   plot_data   = None ,
# )
# END syntax
) :
   assert pdf_file is not None
   assert plot_data is not None
   #
   # age_min, age_max, time_min, time_max
   age_min  = plot_limit['age_min']
   age_max  = plot_limit['age_max']
   time_min = plot_limit['time_min']
   time_max = plot_limit['time_max']
   #
   # color_style_list, n_color_style
   color_style_list = [
      ('blue',       'dashed'),  ('lightblue',  'solid'),
      ('red',        'dashed'),  ('pink',       'solid'),
      ('green',      'dashed'),  ('lightgreen', 'solid'),
      ('black',      'dashed'),  ('gray',       'solid'),
      ('brown',      'dashed'),  ('sandybrown', 'solid'),
      ('darkorange', 'dashed'),  ('gold',       'solid'),
      ('purple',     'dashed'),  ('violet',     'solid'),
   ]
   n_color_style = len( color_style_list )
   #
   # pdf
   pdf = matplotlib.backends.backend_pdf.PdfPages(pdf_file)
   #
   # z_name
   for z_name in plot_data :
      #
      # z_data
      z_data = plot_data[z_name]
      #
      # has_std
      has_std = 'std' in z_data[0]
      #
      # n_age, n_time
      time_list = dict()
      for row in z_data :
         age  = row['age']
         time = row['time']
         if age not in time_list :
            time_list[age] = list()
         time_list[age].append( time )
      last_age = None
      for age in time_list :
         time_list[age] = sorted( time_list[age] )
         last_age       = age
      for age in time_list :
         if time_list[age] != time_list[last_age] :
            msg  = f'plot_curve: plot_data[{z_name}]:'
            msg += 'data is not on a rectangular grid'
            assert False, msg
         if len( time_list[age] ) != len( set( time_list[age] ) ) :
            msg  = f'plot_curve: plot_data[{z_name}]: '
            msg += 'a time is paired with the same age more than once'
            assert False, msg
      n_age  = len( time_list )
      n_time = len( time_list[last_age] )
      #
      # quad_list
      quad_list  = list()
      for row in z_data :
         age   = row['age']
         time  = row['time']
         value = row['value']
         if has_std :
            std = row['std']
         else :
            std = None
         quad_list.append( (age, time, value, std) )
      #
      # quad_list
      # sort first by age and then by time
      quad_list = sorted(quad_list)
      #
      # age, time, value, std
      # initialize these meshgrids as zero
      age   = numpy.zeros( (n_age, n_time), dtype = float)
      time  = numpy.zeros( (n_age, n_time), dtype = float)
      value = numpy.zeros( (n_age, n_time), dtype = float)
      if has_std :
         std   = numpy.zeros( (n_age, n_time), dtype = float)
      for i in range(n_age) :
         for j in range(n_time) :
            #
            # quad corresponding to (i, j) grid point
            k     = i * n_time + j
            quad  = quad_list[k]
            #
            # age, time, value, std for (i, j) grid point
            age[i, j]   = quad[0]
            time[i, j]  = quad[1]
            value[i, j] = quad[2]
            if has_std :
               std[i, j]   = quad[3]
      #
      # age_min, age_max
      age_min = numpy.min(age)
      age_max = numpy.max(age)
      #
      # time_min, time_max
      time_min = numpy.min(time)
      time_max = numpy.max(time)
      #
      # value_min, value_max
      value_max  = numpy.max(value) * 1.05
      value_min  = numpy.min(value) * 0.95
      value_min  = max(value_min , value_max * 1e-6)
      assert value_max > 0.0
      #
      # std_min, std_max
      if  has_std :
         std_max   = numpy.max(std) * 1.05
         std_min   = numpy.min(std) * 0.95
         std_min   = max(std_min, std_max * 1e-5)
         assert std_max > 0.0
      #
      # n_subplot
      if has_std :
         n_subplot = 2
      else :
         n_subplot = 1
      #
      # ********************************************************************
      # for each time, plot value and possibly std as a function of age
      # ********************************************************************
      #
      # n_fig, n_line_per_fig
      n_fig          = math.ceil( n_time / ( n_color_style - 1) )
      n_line_per_fig = math.ceil( n_time / n_fig )
      assert n_line_per_fig < n_color_style
      #
      # initialize color index
      color_index = -1
      #
      for i_fig in range( n_fig ) :
         #
         # save_color_index
         # save for possible re-use by second subplot
         save_color_index = color_index
         #
         # fig
         fig    = new_figure()
         fig.subplots_adjust( hspace = .01 )
         #
         # axis
         # axis for subplot and title for figure
         axis   = pyplot.subplot(n_subplot, 1, 1)
         if plot_title is None :
            axis.set_title( f'{z_name}' )
         else :
            axis.set_title( f'{plot_title}: {z_name}' )
         #
         # start, color_index, stop
         start  = i_fig * n_line_per_fig
         if i_fig > 0 :
            # overlap figures by one line that does not get counted
            # in value of n_line_per_fig
            start        = start - 1
            color_index -= 1
         stop   = min(n_time, start + n_line_per_fig )
         #
         for j in range(start, stop) :
            # color_index
            color_index    = (color_index + 1) % n_color_style
            #
            # color, style
            (color, style) = color_style_list[color_index]
            #
            # x, y
            x   = age[:,j]
            y   = value[:,j]
            # avoid y values less than or equal zero
            y     = numpy.maximum(y, value_min)
            #
            # x, y
            # extend as constant to min and max age
            x     = [age_min] + x.tolist() + [age_max]
            y     = [y[0]]    + y.tolist() + [y[-1]]
            #
            # label
            # used by legend
            label = str( time[0,j] )
            #
            # plot this line
            pyplot.plot(x, y, label=label, color=color, linestyle=style)
            #
            # axis labels
            pyplot.xlabel('age')
            pyplot.ylabel(z_name)
            #
            # y axis sacling and limits
            pyplot.yscale('log')
            pyplot.ylim(value_min, value_max)
         #
         # plot age grid values as vertical lines
         for i in range(n_age) :
            x = age[i, 0]
            pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
         #
         # Shrink current axis by 15% and place legend to right
         box = axis.get_position()
         axis.set_position([
            box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
         ])
         axis.legend(
            title = 'time', loc='center left', bbox_to_anchor=(1, 0.5)
         )
         # ---------------------------------------------------------------
         if has_std :
            # restart colors so they are the same as for the first subplot
            # (we only need on legend for both subplots)
            color_index = save_color_index
            #
            # axis for subplot (uses the same title as the figure)
            axis = pyplot.subplot(n_subplot, 1, 2)
            #
            #
            # start, color_index, stop
            start  = i_fig * n_line_per_fig
            if i_fig > 0 :
               # overlap figures by one line that does not get counted
               # in value of n_line_per_fig
               start        = start - 1
               color_index -= 1
            stop   = min(n_time, start + n_line_per_fig )
            #
            for j in range(start, stop) :
               # color_index
               color_index    = (color_index + 1) % n_color_style
               #
               # color, style
               (color, style) = color_style_list[color_index]
               #
               # x, y
               x   = age[:,j]
               y   = std[:,j]
               # avoid y values less than or equal zero
               y     = numpy.maximum(y, std_min)
               #
               # x, y
               # extend as constant to min and max age
               x     = [age_min] + x.tolist() + [age_max]
               y     = [y[0]]    + y.tolist() + [y[-1]]
               #
               # label
               # used by legend
               label = str( time[0,j] )
               #
               # plot this line
               pyplot.plot(x, y, label=label, color=color, linestyle=style)
               #
               #
               # axis labels
               pyplot.xlabel('age')
               pyplot.ylabel('standard error')
               #
               # y axis sacling and limits
               pyplot.yscale('log')
               pyplot.ylim(std_min, std_max)
            #
            # plot age grid values as vertical lines
            for i in range(n_age) :
               x = age[i, 0]
               pyplot.axvline(
                  x, color='black', linestyle='dotted', alpha=0.3
               )
            #
            # Shrink current axis by 15% and place legend to right
            box = axis.get_position()
            axis.set_position([
               box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
            ])
         # ----------------------------------------------------------------
         pdf.savefig( fig )
      # *******************************************************************-
      # for each age, plot value and possibly std as a function of time
      # ********************************************************************
      #
      # n_fig, n_line_per_fig
      n_fig          = math.ceil( n_age / ( n_color_style - 1) )
      n_line_per_fig = math.ceil( n_age / n_fig )
      assert n_line_per_fig < n_color_style
      #
      # initialize color index
      color_index = -1
      #
      for i_fig in range( n_fig ) :
         #
         # save_color_index
         # save for possible re-use by second subplot
         save_color_index = color_index
         #
         # fig
         fig    = new_figure()
         fig.subplots_adjust( hspace = .01 )
         #
         # axis
         # axis for subplot and title for figure
         axis   = pyplot.subplot(n_subplot, 1, 1)
         if plot_title is None :
            axis.set_title( f'{z_name}' )
         else :
            axis.set_title( f'{plot_title}: {z_name}' )
         #
         # start, color_index, stop
         start  = i_fig * n_line_per_fig
         if i_fig > 0 :
            # overlap figures by one line that does not get counted
            # in value of n_line_per_fig
            start        = start - 1
            color_index -= 1
         stop   = min(n_age, start + n_line_per_fig )
         #
         for i in range(start, stop) :
            # color_index
            color_index    = (color_index + 1) % n_color_style
            #
            # color, style
            (color, style) = color_style_list[color_index]
            #
            # x, y
            x   = time[i,:]
            y   = value[i,:]
            # avoid y values less than or equal zero
            y     = numpy.maximum(y, value_min)
            #
            # x, y
            # extend as constant to min and max age
            x     = [time_min] + x.tolist() + [time_max]
            y     = [y[0]]     + y.tolist() + [y[-1]]
            #
            # label
            # used by legend
            label = str( age[i,0] )
            #
            # plot this line
            pyplot.plot(x, y, label=label, color=color, linestyle=style)
            #
            # axis labels
            pyplot.xlabel('time')
            pyplot.ylabel(z_name)
            #
            # y axis sacling and limits
            pyplot.yscale('log')
            pyplot.ylim(value_min, value_max)
         #
         # plot time grid values as vertical lines
         for j in range(n_time) :
            x = time[0, j]
            pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
         #
         # Shrink current axis by 15% and place legend to right
         box = axis.get_position()
         axis.set_position([
            box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
         ])
         axis.legend(
            title = 'age', loc='center left', bbox_to_anchor=(1, 0.5)
         )
         # ---------------------------------------------------------------
         if has_std :
            # restart colors so they are the same as for the first subplot
            # (we only need on legend for both subplots)
            color_index = save_color_index
            #
            # axis for subplot (uses the same title as the figure)
            axis = pyplot.subplot(n_subplot, 1, 2)
            #
            #
            # start, color_index, stop
            start  = i_fig * n_line_per_fig
            if i_fig > 0 :
               # overlap figures by one line that does not get counted
               # in value of n_line_per_fig
               start        = start - 1
               color_index -= 1
            stop   = min(n_age, start + n_line_per_fig )
            #
            for i in range(start, stop) :
               # color_index
               color_index    = (color_index + 1) % n_color_style
               #
               # color, style
               (color, style) = color_style_list[color_index]
               #
               # x, y
               x   = time[i,:]
               y   = std[i,:]
               # avoid y values less than or equal zero
               y     = numpy.maximum(y, std_min)
               #
               # x, y
               # extend as constant to min and max age
               x     = [time_min] + x.tolist() + [time_max]
               y     = [y[0]]    + y.tolist() + [y[-1]]
               #
               # label
               # used by legend
               label = str( age[i,0] )
               #
               # plot this line
               pyplot.plot(x, y, label=label, color=color, linestyle=style)
               #
               #
               # axis labels
               pyplot.xlabel('time')
               pyplot.ylabel('standard error')
               #
               # y axis sacling and limits
               pyplot.yscale('log')
               pyplot.ylim(std_min, std_max)
            #
            # plot time grid values as vertical lines
            for j in range(n_time) :
               x = time[0, j]
               pyplot.axvline(
                  x, color='black', linestyle='dotted', alpha=0.3
               )
            #
            # Shrink current axis by 15% and place legend to right
            box = axis.get_position()
            axis.set_position([
               box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
            ])
         # -----------------------------------------------------------------
         pdf.savefig( fig )
   # *************************************************************************
   #
   # end of pages in pdf file
   pdf.close()
