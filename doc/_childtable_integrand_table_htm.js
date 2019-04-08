// Child table for section integrand_table
document.write('\
<select onchange="integrand_table_child(this)">\
<option>integrand_table-&gt;</option>\
<option>integrand_table.py</option>\
</select>\
');
function integrand_table_child(item)
{	var child_list = [
		'integrand_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
