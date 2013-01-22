//(function(){

//rps data
var rps_data = [];
var rps_max_data_len = 10;
var anim_handle = 0;

//helper function=====================================================
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

function defaultErrProc(err) {
    if (err == 2){
        $("#login_label").text("No login");
        $("#btn_logout").hide();
        delCookie("usertoken");
        delCookie("username");
    }
}

(function() {
    var lastTime = 0;
    var vendors = ['ms', 'moz', 'webkit', 'o'];
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
        window.cancelAnimationFrame = 
          window[vendors[x]+'CancelAnimationFrame'] || window[vendors[x]+'CancelRequestAnimationFrame'];
    }
 
    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function() { callback(currTime + timeToCall); }, 
              timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };
 
    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        };
}());

var Base64Binary = {
    _keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",

    /* will return a  Uint8Array type */
    decodeArrayBuffer: function(input) {
        var bytes = (input.length/4) * 3;
        var ab = new ArrayBuffer(bytes);
        this.decode(input, ab);

        return ab;
    },

    decode: function(input, arrayBuffer) {
        //get last chars to see if are valid
        var lkey1 = this._keyStr.indexOf(input.charAt(input.length-1));      
        var lkey2 = this._keyStr.indexOf(input.charAt(input.length-2));      

        var bytes = (input.length/4) * 3;
        if (lkey1 == 64) bytes--; //padding chars, so skip
        if (lkey2 == 64) bytes--; //padding chars, so skip

        var uarray;
        var chr1, chr2, chr3;
        var enc1, enc2, enc3, enc4;
        var i = 0;
        var j = 0;

        if (arrayBuffer)
            uarray = new Uint8Array(arrayBuffer);
        else
            uarray = new Uint8Array(bytes);

        input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");

        for (i=0; i<bytes; i+=3) {  
            //get the 3 octects in 4 ascii chars
            enc1 = this._keyStr.indexOf(input.charAt(j++));
            enc2 = this._keyStr.indexOf(input.charAt(j++));
            enc3 = this._keyStr.indexOf(input.charAt(j++));
            enc4 = this._keyStr.indexOf(input.charAt(j++));

            chr1 = (enc1 << 2) | (enc2 >> 4);
            chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
            chr3 = ((enc3 & 3) << 6) | enc4;

            uarray[i] = chr1;           
            if (enc3 != 64) uarray[i+1] = chr2;
            if (enc4 != 64) uarray[i+2] = chr3;
        }

        return uarray;  
    }
}

//=====================================================
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
    $("#btn_dig").click(function() {
        window.location.href="dig.html";
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
    var alt = $("#rps_alert");
    $.getJSON("/cmapi/rps", {data:data}, function(json){
        var err = json.error;
        if (err==0) {
            bsalert(alt, "success", "success");
            $("#rps_msg").text("opponame:"+json.opponame+", rps:"+json.rps);

            $("#rps_modal").modal( {
                keyboard: false,
                backdrop: "static"});
        } else {
            defaultErrProc(err);
            bsalert(alt, "error", "rps error:"+err);
        }
        clearTimeout(timeout);
        btn.button("reset");
    })
    .error(function(){
        clearTimeout(timeout);
        btn.button("reset");
        bsalert(alt, "error", "net error");
    });

    btn.button("loading");
}

function btn_modal_ok() {
    console.log(modal, ok);
}

function get_random_color() {
    var letters = '0123456789ABCDEF'.split('');
    var color = '#';
    for (var i = 0; i < 6; i++ ) {
        color += letters[Math.round(Math.random() * 15)];
    }
    return color;
}

var _t = 0;
function draw(t) {
    var canvas = $("#dig_canvas")[0];
    var ctx = canvas.getContext("2d");
    var w = canvas.width;
    var h = canvas.height;
    var d = 16;
    var _d = d-1;

    _t += 0.1;
    ctx.clearRect(0, 0, w, h);
    var x0 = Math.sin(_t)*10;
    anim_handle = requestAnimationFrame(draw);
}

//})()