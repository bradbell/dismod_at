// Child table for section covariate_table
document.write('\
<select onchange="covariate_table_child(this)">\
<option>covariate_table-&gt;</option>\
<option>covariate_table.py</option>\
</select>\
');
function covariate_table_child(item)
{	var child_list = [
		'covariate_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
