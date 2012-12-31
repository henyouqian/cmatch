function bsalert(alert, type, text){
    alert.text(text);
    if ( type == 'success' ){
        alert.attr('class', 'alert alert-success');
    }else if ( type == 'error' ){
        alert.attr('class', 'alert alert-error');
    }else if ( type == 'warning' ){
        alert.attr('class', 'alert');
    }
}

$(document).ready(function(){
    var navli = $('.nav-list li');
    navli.click(function(e) {
        navli.removeClass('active');
        var $this = $(this);
        if (!$this.hasClass('active')) {
            $this.addClass('active');
            $this.children()
        }
        location = $this.children('a:first').attr('href');
        e.preventDefault();
    });

    $('#btn_register').click(function() {
        register();
    });
    $(".reg_input").keypress(function(event) {
        if ( event.which == 13 ) {
            register();
        }
    });

    $('#btn_login').click(function() {
        login();
    });
    $(".login_input").keypress(function(event) {
        if ( event.which == 13 ) {
            login();
        }
    });

    $('body').ajaxError(function() {
      alert('ajax error');
    });
});

function register(){
    var username=$('#reg_username').attr('value');
    var password=$('#reg_password').attr('value');
    $.getJSON('/cmapi/register', {username:username, password:password}, function(json){
        var err = json.error;
        var alert = $("#reg_alert");
        alert.fadeIn('fast');
        if (err==0){
            bsalert(alert, 'success', 'Sign up succeed!');
        }else{
            bsalert(alert, 'error', 'Sign up failed! error:'+err);
        }
    });
}

function login(){
    var username=$('#login_username').attr('value');
    var password=$('#login_password').attr('value');
    $.getJSON('/cmapi/login', {username:username, password:password}, function(json){
        var err = json.error;
        var alert = $("#login_alert");
        var username = json.username;
        alert.fadeIn('fast');
        if (err==0){
            bsalert(alert, 'success', 'Sign in succeed!');
            $('#login_label').text('Logged in as: '+username);
        }else{
            bsalert(alert, 'error', 'Sign in failed:'+err);
        }
    });
}