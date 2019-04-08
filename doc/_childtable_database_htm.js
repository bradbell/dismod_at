// Child table for section database
document.write('\
<select onchange="database_child(this)">\
<option>database-&gt;</option>\
<option>input</option>\
<option>data_flow</option>\
</select>\
');
function database_child(item)
{	var child_list = [
		'input.htm',
		'data_flow.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
