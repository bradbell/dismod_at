// Child table for section age_table
document.write('\
<select onchange="age_table_child(this)">\
<option>age_table-&gt;</option>\
<option>age_table.py</option>\
</select>\
');
function age_table_child(item)
{	var child_list = [
		'age_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
