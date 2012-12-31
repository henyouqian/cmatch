$(document).ready(function(){
	$('#btn_register').click(function() {
		register();
	});
	$(".input-block-level").keypress(function(event) {
		if ( event.which == 13 ) {
			register();
		}
	});
});

function register(){
	var username=$('#username').attr('value');
	var password=$('#password').attr('value');
	$.getJSON('/cmapi/register', {username:username, password:password}, function(json){
		var err = json.error;
		var alert = $("#alert");
		alert.fadeIn('fast');
		if (err==0){
			alert.text('Sign up succeed!');
			alert.removeClass('alert-error').addClass('alert-success');
		}else{
			alert.text('Sign up failed!');
			alert.removeClass('alert-success').addClass('alert-error');
		}
	});
}