$(document).ready(function(){
	$('#btn_login').click(function() {
		login();
	});
	$('.input-block-level').keypress(function(event) {
		if ( event.which == 13 ) {
			login();
		}
	});
});

function login(){
	var username=$('#username').attr('value');
	var password=$('#password').attr('value');
	$.getJSON('/cmapi/login', {username:username, password:password}, function(json){
		var err = json.error;
		var alert = $("#alert");
		var username = json.username;
		alert.fadeIn('fast');
		if (err==0){
			alert.text('Sign in succeed: '+username);
			alert.removeClass('alert-error').addClass('alert-success');
		}else{
			alert.text('Sign in failed!');
			alert.removeClass('alert-success').addClass('alert-error');
		}
	});
}