// Child table for section rate_table
document.write('\
<select onchange="rate_table_child(this)">\
<option>rate_table-&gt;</option>\
<option>rate_table.py</option>\
</select>\
');
function rate_table_child(item)
{	var child_list = [
		'rate_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
