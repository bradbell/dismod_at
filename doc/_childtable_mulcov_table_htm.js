// Child table for section mulcov_table
document.write('\
<select onchange="mulcov_table_child(this)">\
<option>mulcov_table-&gt;</option>\
<option>mulcov_table.py</option>\
</select>\
');
function mulcov_table_child(item)
{	var child_list = [
		'mulcov_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
