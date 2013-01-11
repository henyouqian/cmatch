var map;
function canvasAdj() {
	var maprect = $("#map_canvas").offset();
	var jqcanvas = $("#dig_canvas");
  	jqcanvas.css("left", maprect.left).css("top", maprect.top);
}

var canvas = $("#dig_canvas")[0];
var ctx = canvas.getContext("2d");
var canvasW = canvas.width;
var canvasH = canvas.height;

function initialize() {
	var mapOptions = {
  		center: new google.maps.LatLng(30.275672,120.176491),
  		zoom: 8,
  		mapTypeId: google.maps.MapTypeId.ROADMAP,
  		disableDefaultUI: true,
	};
	map = new google.maps.Map(document.getElementById("map_canvas"),
    mapOptions);

	window.map = map;
}

function getPos() {
	var proj = map.getProjection();
	var ll = map.getCenter();
	console.log(ll);
	var point = proj.fromLatLngToPoint(ll);
	return point;
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

function draw() {
	canvasAdj();

	var zoom = map.getZoom();
	zoom = Math.max(10, Math.min(18, zoom));
	var d = 8*Math.pow(2, (zoom-10));

	ctx.clearRect(0, 0, canvasW, canvasH);
	ctx.fillStyle="#5500FF";
	ctx.fillRect(0, 0, d, d);

	anim_handle = requestAnimationFrame(draw);
}

$(document).ready(function(){
	initialize();

	Mousetrap.bind(["up", "w"], function() {
		map.setZoom(map.getZoom()+1);
	    $("#info_div").text("zoom:"+map.getZoom());
		return false;
	}, 'keydown');

	Mousetrap.bind(["down", "s"], function() {
		map.setZoom(map.getZoom()-1);
	    $("#info_div").text("zoom:"+map.getZoom());
	}, 'keydown');
	Mousetrap.bind(["left", "a"], function() {
		map.panBy(-1000, 0);
	}, 'keydown');
	Mousetrap.bind(["right", "d"], function() {
		map.panBy(1, 0);
	}, 'keydown');


	var draging = false;
	var prevX = 0;
	var prevY = 0;
	$("#dig_canvas").mousedown(function(event) {
		draging = true;
		prevX = event.pageX;
		prevY = event.pageY;
		return false;
	});
	$("#dig_canvas").mousemove(function(event) {
		if (draging) {
			var dx = event.pageX - prevX;
			var dy = event.pageY - prevY;
			map.panBy(-dx, -dy);
			prevX = event.pageX;
			prevY = event.pageY;
		}
		return false;
	});
	$("#dig_canvas").mouseup(function(event) {
		draging = false;
		return false;
	});
	$("#dig_canvas").mouseout(function(event) {
		draging = false;
		return false;
	});

	$("#dig_canvas").mousewheel(function(event, delta) {
		map.setZoom(map.getZoom()+delta);
	    $("#info_div").text("zoom:"+map.getZoom());
	    return false;
	});

	$(window).resize(function() {
		canvasAdj();
		return false;
	});

	draw();
});