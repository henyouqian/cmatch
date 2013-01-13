var map;
var tileData = [];
var centerX, centerY;
var TILE_MAX = 32768;
var canvas = $("#dig_canvas")[0];
var ctx = canvas.getContext("2d");
var canvasW = canvas.width;
var canvasH = canvas.height;
var moveUp = false;
var moveDown = false;
var moveLeft = false;
var moveRight = false;

function canvasAdj() {
	var maprect = $("#map_canvas").offset();
	var jqcanvas = $("#dig_canvas");
  	jqcanvas.css("left", maprect.left).css("top", maprect.top);
}

function getCenter() {
	var proj = map.getProjection();
	var ll = map.getCenter();
	var point = proj.fromLatLngToPoint(ll);
	var a = TILE_MAX/256.0;
	point.x *= a;
	point.y *= a;
	return {"x":point.x, "y":point.y};
}

function pos2ll(x, y) {
	x %= TILE_MAX;
	y %= TILE_MAX;
	var a = 256.0/TILE_MAX;
	x *= a;
	y *= a;
	var proj = map.getProjection();
	var ll = proj.fromPointToLatLng(new google.maps.Point(x, y), false);
	return ll;
}

function ll2pos(ll) {
	var proj = map.getProjection();
	var point = proj.fromLatLngToPoint(ll);
	var a = TILE_MAX/256.0;
	point.x *= a;
	point.y *= a;
	return {"x":point.x, "y":point.y};
}

//[0.0,32768.0)
function setCenter(x, y) {
	var ll = pos2ll(x, y);
	map.setCenter(ll);
	centerX = x;
	centerY = y;
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

function drawBlock(canvasX, canvasY, blockX, blockY, tileLen) {
	var row, col, x, y;
	for (row = 0, y = canvasY; row < 16; ++row, y += tileLen) {
		if (y > canvasH)
			break;
		if (y + tileLen < 0)
			continue;
		for (col = 0, x = canvasX; col < 16; ++col, x += tileLen) {
			if (x > canvasW)
				break;
			if (x + tileLen < 0)
				continue;
			ctx.fillRect(x, y, tileLen, tileLen);
		}
	}
}

function frame() {
	update();
	draw();
	frame_handle = requestAnimationFrame(frame);
}

function update() {
	var x = 0;
	var y = 0;
	var speed = 0.2;
	if (moveLeft && !moveRight)
		x = -1;
	else if (moveRight && !moveLeft)
		x = 1;
	if (moveUp && !moveDown)
		y = -1;
	else if (moveDown && !moveUp)
		y = 1;

	if (x == 0 && y == 0)
		return;
	else if (x != 0 && y != 0)
		speed *= 0.7071;

	var zoom = map.getZoom();
	if (zoom > 10)
		speed /= Math.pow(2, (zoom-10));
	setCenter(centerX+speed*x, centerY+speed*y);
}

function draw() {
	canvasAdj();
	var zoom = map.getZoom();
	ctx.clearRect(0, 0, canvasW, canvasH);
	if (zoom >= 10 && zoom <= 18) {
		var tileLen = 8*Math.pow(2, (zoom-10));
		var blockLen = tileLen * 16;
		var left = centerX - canvasW*0.5/tileLen;
		var top = centerY - canvasH*0.5/tileLen;

		var canvasX = Math.floor(-(left%16)*tileLen);
		var canvasY = Math.floor(-(top%16)*tileLen);
		var blockX = Math.floor(left/16);
		var blockY = Math.floor(top/16);
		binfo = [canvasX, canvasY, blockX, blockY];

		ctx.globalAlpha = 0.3;
		ctx.fillStyle="#FF0000";

		for (var y = canvasY, blkY=blockY; ; y += blockLen, ++blkY) {
			if (y > canvasH)
				break;
			if (y + blockLen < 0)
				continue;
			for (var x = canvasX, blkX=blockX; ; x += blockLen, ++blkX) {
				if (x > canvasW)
					break;
				if (x + blockLen < 0)
					continue;
				drawBlock(x, y, blkX, blkY, tileLen);
			}
		}

		//draw center highlight frame
		var x = Math.floor(canvasW*0.5-(centerX % 1)*tileLen);
		var y = Math.floor(canvasH*0.5-(centerY % 1)*tileLen);
		ctx.fillRect(x+1, y+1, tileLen-2, tileLen-2);
		ctx.globalAlpha = 1.0;
		ctx.strokeStyle="#000000";
		//ctx.strokeRect(x, y, tileLen, tileLen);
	}
}

$(document).ready(function(){
	var mapOptions = {
  		zoom: 10,
  		mapTypeId: google.maps.MapTypeId.ROADMAP,
  		disableDefaultUI: true,
	};
	map = new google.maps.Map(document.getElementById("map_canvas"), mapOptions);
	window.map = map;

	//callback
	google.maps.event.addListenerOnce(map, 'projection_changed', function(){
	    setCenter(27322.731269, 13490.250045);
	    frame();
	});

	//zoom
	Mousetrap.bind(["up"], function() {
		map.setZoom(map.getZoom()+1);
	    $("#info_div").text("zoom:"+map.getZoom());
		return false;
	}, 'keydown');

	Mousetrap.bind(["down"], function() {
		map.setZoom(map.getZoom()-1);
	    $("#info_div").text("zoom:"+map.getZoom());
	    return false;
	}, 'keydown');

	//move
	Mousetrap.bind(["w"], function() {
		moveUp = true;
	}, 'keydown');
	Mousetrap.bind(["s"], function() {
		moveDown = true;
	}, 'keydown');
	Mousetrap.bind(["a"], function() {
		moveLeft = true;
	}, 'keydown');
	Mousetrap.bind(["d"], function() {
		moveRight = true;
	}, 'keydown');

	Mousetrap.bind(["w"], function() {
		moveUp = false;
	}, 'keyup');
	Mousetrap.bind(["s"], function() {
		moveDown = false;
	}, 'keyup');
	Mousetrap.bind(["a"], function() {
		moveLeft = false;
	}, 'keyup');
	Mousetrap.bind(["d"], function() {
		moveRight = false;
	}, 'keyup');

	//mouse
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

			var proj = map.getProjection();
			var ll = map.getCenter();
			var point = proj.fromLatLngToPoint(ll);
			var a = TILE_MAX/256.0;
			centerX = point.x * a;
			centerY = point.y * a;
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
});