// Child table for section whats_new_2019
document.write('\
<select onchange="whats_new_2019_child(this)">\
<option>whats_new_2019-&gt;</option>\
<option>whats_new_2018</option>\
<option>whats_new_2017</option>\
<option>whats_new_2016</option>\
<option>whats_new_2015</option>\
</select>\
');
function whats_new_2019_child(item)
{	var child_list = [
		'whats_new_2018.htm',
		'whats_new_2017.htm',
		'whats_new_2016.htm',
		'whats_new_2015.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
