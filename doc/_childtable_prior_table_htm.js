// Child table for section prior_table
document.write('\
<select onchange="prior_table_child(this)">\
<option>prior_table-&gt;</option>\
<option>prior_table.py</option>\
</select>\
');
function prior_table_child(item)
{	var child_list = [
		'prior_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
