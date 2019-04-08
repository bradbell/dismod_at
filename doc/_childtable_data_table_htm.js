// Child table for section data_table
document.write('\
<select onchange="data_table_child(this)">\
<option>data_table-&gt;</option>\
<option>data_table.py</option>\
</select>\
');
function data_table_child(item)
{	var child_list = [
		'data_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
