//(function(){

//cookie
function getCookie(c_name) {
    if (document.cookie.length>0) {
        c_start=document.cookie.indexOf(c_name + "=");
        if (c_start!=-1) { 
            c_start=c_start + c_name.length+1;
            c_end=document.cookie.indexOf(";",c_start);
            if (c_end==-1) c_end=document.cookie.length;
            return unescape(document.cookie.substring(c_start,c_end))
        }
    }
    return "";
}

function setCookie(c_name,value,expiredays) {
    var exdate = new Date();
    exdate.setDate(exdate.getDate()+expiredays);
    document.cookie=c_name+ "=" +escape(value)+
        ((expiredays==null) ? "" : ";expires="+exdate.toGMTString());
}

function delCookie(c_name){
    setCookie(c_name, 0, -1);
}

//rps data
var rps_data = [];
var rps_max_data_len = 10;

function bsalert(alert, type, text) {
    alert.text(text);
    if (type == "success") {
        alert.attr("class", "alert alert-success");
    }else if (type == "error") {
        alert.attr("class", "alert alert-error");
    }else if (type == "warning") {
        alert.attr("class", "alert");
    }
    alert.fadeIn("fast");
}

$(document).ready(function(){
    //ajax error
    // $("body").ajaxError(function() {
    //   alert("ajax error");
    // });

    //try relogin
    relogin();

    //navlist goto
    var navli = $(".nav-list li");
    navli.click(function(e) {
        navli.removeClass("active");
        var $this = $(this);
        if (!$this.hasClass("active")) {
            $this.addClass("active");
            $this.children()
        }
        location = $this.children("a:first").attr("href");
        e.preventDefault();
    });

    //register
    $("#btn_register").click(function() {
        register();
    });
    $(".reg_input").keypress(function(event) {
        if ( event.which == 13 ) {
            register();
        }
    });

    //login
    $("#btn_login").click(function() {
        login();
    });
    $(".login_input").keypress(function(event) {
        if ( event.which == 13 ) {
            login();
        }
    });

    //logout
    $("#btn_logout").click(function() {
        logout();
    });

    //rps
    $("#btn_rock").click(function() {
        btn_rock();
    });
    $("#btn_paper").click(function() {
        btn_paper();
    });
    $("#btn_scissors").click(function() {
        btn_scissors();
    });
    $("#btn_del").click(function() {
        btn_del();
    });
    $("#btn_clear").click(function() {
        btn_clear();
    });
    $("#btn_battle").click(function() {
        var btn = $(this);
        btn_battle(btn);
    });
    
});

function register() {
    var username=$("#reg_username").attr("value");
    var password=$("#reg_password").attr("value");
    var alert = $("#reg_alert");
    $.getJSON("/cmapi/register", {username:username, password:password}, function(json) {
        var err = json.error;
        if (err==0) {
            bsalert(alert, "success", "Sign up succeed!");
        }else{
            bsalert(alert, "error", "Sign up failed! error:"+err);
        }
    })
    .error(function(){
        bsalert(alert, "error", "net error");
    });
}

function login() {
    var username=$("#login_username").attr("value");
    var password=$("#login_password").attr("value");
    var alert = $("#login_alert");
    $.getJSON("/cmapi/login", {username:username, password:password}, function(json){
        var err = json.error;
        var username = getCookie("username");
        if (err==0) {
            bsalert(alert, "success", "Sign in succeed!");
            $("#login_label").text("Logged in as: "+username);
            $("#btn_logout").show();
        } else {
            bsalert(alert, "error", "Sign in failed:"+err);
        }
    })
    .error(function(){
        bsalert(alert, "error", "net error");
    });
}

function relogin() {
    var usertoken = getCookie("usertoken");
    if (usertoken) {
        $.getJSON("/cmapi/relogin", function(json){
            var err = json.error;
            if (err==0) {
                var username = getCookie("username");
                $("#login_label").text("Logged in as: "+username);
                $("#btn_logout").show();
            } else {
                delCookie("usertoken");
                delCookie("username");
            }
        });
    }
}

function logout() {
    var usertoken = getCookie("usertoken");
    if (usertoken) {
        $.getJSON("/cmapi/logout", {usertoken:usertoken});
    }
    $("#login_label").text("No login");
    $("#btn_logout").hide();
    delCookie("usertoken");
    delCookie("username");
}

//=======================================
//rps
function btn_rock() {
    if (rps_data.length < rps_max_data_len) {
        rps_data.push("r");
        $("#rps_well").append("<button class='btn btn-info' disabled=''>R</button> ");
        if (rps_data.length == rps_max_data_len) {
            $("#btn_battle").fadeIn("fast");
        }
    }
}

function btn_paper() {
    if (rps_data.length < rps_max_data_len) {
        rps_data.push("p");
        $("#rps_well").append("<button class='btn btn-success' disabled=''>P</button> ");
        if (rps_data.length == rps_max_data_len) {
            $("#btn_battle").fadeIn("fast");
        }
    }
}

function btn_scissors() {
    if (rps_data.length < rps_max_data_len) {
        rps_data.push("s");
        $("#rps_well").append("<button class='btn btn-warning' disabled=''>S</button> ");
        if (rps_data.length == rps_max_data_len) {
            $("#btn_battle").fadeIn("fast");
        }
    }
}

function btn_del() {
    rps_data.pop();
    $("#rps_well>button").remove(":last-child");
    $("#btn_battle").fadeOut("fast");
}

function btn_clear() {
    rps_data = [];
    $("#rps_well").empty();
    $("#btn_battle").fadeOut("fast");
}

function btn_battle(btn) {
    var timeout = setTimeout(function () {
      btn.button("reset")
    }, 3000);

    var data = rps_data.join("");
    if (data.length != rps_max_data_len) {
        alert("data.length != rps_max_data_len");
        return;
    }
    var alert = $("#rps_alert");
    $.getJSON("/cmapi/rps", {data:data}, function(json){
        var err = json.error;
        if (err==0) {
            bsalert(alert, "success", "success");
            $("#rps_modal").modal( {
                keyboard: false,
                backdrop: "static"});
            //todo
        } else {
            bsalert(alert, "error", "rps error:"+err);
        }
        clearTimeout(timeout);
        btn.button("reset");
    })
    .error(function(){
        bsalert(alert, "error", "net error");
    });

    btn.button("loading");
}

function btn_modal_ok() {
    console.log(modal, ok);
}

//})()