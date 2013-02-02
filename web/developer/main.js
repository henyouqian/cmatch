
$(document).ready(function(){
    $.getJSON("/cmapi/dev/listapps", function(json) {
        var err = json.error;
        if (err==0) {
            var apps = json.apps;
            for (appid in apps) {
                $("#ul_apps").append("<li><a class='a_app' href='#' appid='"+appid+"'>"+apps[appid]+"</a></li>");
            }
        }else{
            alert(err);
        }
        $(".a_app").click(function () {
            var appid = $(this).attr("appid");
            var appname = $(this).text();
            window.location.href="app.html?appid="+appid+"&appname="+appname;
        });
    })

    $("#a_addapp").click(function () {
        $("#addapp_modal").modal( {
            keyboard: false,
            backdrop: "static"}
        );
    });

    
});
