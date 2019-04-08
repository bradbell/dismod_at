// Child table for section metropolis
document.write('\
<select onchange="metropolis_child(this)">\
<option>metropolis-&gt;</option>\
<option>user_metropolis.py</option>\
</select>\
');
function metropolis_child(item)
{	var child_list = [
		'user_metropolis.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
