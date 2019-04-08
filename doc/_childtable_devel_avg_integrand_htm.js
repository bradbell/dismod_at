// Child table for section devel_avg_integrand
document.write('\
<select onchange="devel_avg_integrand_child(this)">\
<option>devel_avg_integrand-&gt;</option>\
<option>avg_integrand_ctor</option>\
<option>avg_integrand_rectangle</option>\
<option>avg_integrand_add_cohort</option>\
</select>\
');
function devel_avg_integrand_child(item)
{	var child_list = [
		'avg_integrand_ctor.htm',
		'avg_integrand_rectangle.htm',
		'avg_integrand_add_cohort.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
