var map;
var tileData = {};
var tileTime = {};
var centerX = -1;
var centerY = -1;
var centerOffsetX = 0;
var centerOffsetY = 0;
var TILE_MAX = 32768;
var TILES_PER_BLOCK_SIDE = 16;
var BLOCK_BYTES = TILES_PER_BLOCK_SIDE * TILES_PER_BLOCK_SIDE / 8;
var ZOOM_LOW = 10;
var ZOOM_HIGH = 16;
var zoom = ZOOM_LOW;
var canvas = $("#dig_canvas")[0];
var ctx = canvas.getContext("2d");
var canvasW = canvas.width;
var canvasH = canvas.height;
var moveUp = false;
var moveDown = false;
var moveLeft = false;
var moveRight = false;
var time;
var showMap = false;
var frame_handle = 0;

function googleExist() {
	if (typeof(google) == "object"){
		return true;
	}
	return false;
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

function canvasAdj() {
	var maprect = $("#map_canvas").offset();
	var jqcanvas = $("#dig_canvas");
  	jqcanvas.css("left", maprect.left).css("top", maprect.top);
}

function printCenter() {
	var proj = map.getProjection();
	var ll = map.getCenter();
	var point = proj.fromLatLngToPoint(ll);
	var a = TILE_MAX/256.0;
	point.x *= a;
	point.y *= a;
	console.log({"x":point.x, "y":point.y});
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
	var blockX1 = Math.floor(centerX / TILES_PER_BLOCK_SIDE);
    var blockY1 = Math.floor(centerY / TILES_PER_BLOCK_SIDE);
    var blockX2 = Math.floor(x / TILES_PER_BLOCK_SIDE);
    var blockY2 = Math.floor(y / TILES_PER_BLOCK_SIDE);

	if (googleExist()) {
		var ll = pos2ll(x, y);
		map.setCenter(ll);
	}
	centerX = x;
	centerY = y;

	if (blockX1 != blockX2 || blockY1 != blockY2) {
    	fetchBlocks();
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

var _keyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

/* will return a  Uint8Array type */
function unbase64(input, arrayBuffer) {
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

function getTileLen(){
	return 8*Math.pow(2, (zoom-ZOOM_LOW));
}

var lastFetchBlockTime = 0;
function fetchBlocks(notry) {
	var t = new Date().getTime();
	var timeInterval = 1000;
	if (t < lastFetchBlockTime + timeInterval) {
		if (typeof(notry) == "undefined")
			setTimeout("fetchBlocks(1);", timeInterval);
			console.log("pending fetch block");
		return;
	}

	lastFetchBlockTime = t;

	var tileLen = 8;
	var blockLen = tileLen * TILES_PER_BLOCK_SIDE;
	var left = centerX - canvasW*0.5/tileLen;
	var top = centerY - canvasH*0.5/tileLen;

	var canvasX = Math.floor(-(left%TILES_PER_BLOCK_SIDE)*tileLen)-blockLen;
	var canvasY = Math.floor(-(top%TILES_PER_BLOCK_SIDE)*tileLen)-blockLen;
	var blockX = Math.floor(left/TILES_PER_BLOCK_SIDE)-1;
	var blockY = Math.floor(top/TILES_PER_BLOCK_SIDE)-1;

	var x, y, blkX, blkY;
	var data = (Math.floor(centerX)+0.5) + "," + (Math.floor(centerY)+0.5) + ",";
	var blockNum = 0;
	for (y = canvasY, blkY = blockY; ; y += blockLen, ++blkY) {
		if (y > canvasH + blockLen)
			break;
		for (x = canvasX, blkX = blockX; ; x += blockLen, ++blkX) {
			if (x > canvasW + blockLen)
				break;
			// if (typeof(tileData[blkX + "," + blkY]) == "undefined") {
			// 	data += blkX + "," + blkY + ",";
			// 	++i;
			// }
			data += blkX + "," + blkY + ",";
			++blockNum;
		}
	}
	console.log("fetch:"+blockNum);
	if (blockNum > 0) {
		$.post("/cmapi/getblock", data, function(json) {
			var err = json.error;
			if (err==0) {
				for (k in json.data) {
					var v = json.data[k];
					if (v != 0) {
						tileData[k] = unbase64(v);
					}else{
						tileData[k] = 0;
					}
	 			}
			}else{
				
			}
	    }, "json");
	}
}

function drawBlock(canvasX, canvasY, blockX, blockY, tileLen) {
	var row, col, x, y;
	var data = tileData[blockX+","+blockY];
	if (typeof(data) == "undefined" || data == 0) {
		data = new Uint8Array(BLOCK_BYTES);
	}
	
	for (row = 0, y = canvasY; row < TILES_PER_BLOCK_SIDE; ++row, y += tileLen) {
		if (y > canvasH)
			break;
		if (y + tileLen < 0)
			continue;
		for (col = 0, x = canvasX; col < TILES_PER_BLOCK_SIDE; ++col, x += tileLen) {
			if (x > canvasW)
				break;
			if (x + tileLen < 0)
				continue;
			var idx = row*TILES_PER_BLOCK_SIDE+col;
			var bt = data[Math.floor(idx/8)];
			if ((bt>>(7-Math.floor(idx)%8) & 1) == 0) {
				if (row==0 && col==0)
					ctx.fillRect(x+1, y+1, tileLen-1, tileLen-1);
				else 
					ctx.fillRect(x, y, tileLen, tileLen);
			}
		}
	}
}

function dig() {
	var tileX = Math.floor(centerX+centerOffsetX);
	var tileY = Math.floor(centerY+centerOffsetY);
	if (checkTile(tileX, tileY))
		return;

	$.getJSON("/cmapi/dig", {x:tileX, y:tileY}, function(json) {
        var err = json.error;
        if (err==0) {
            var data = unbase64(json.data);
            var blockX = Math.floor(tileX / TILES_PER_BLOCK_SIDE);
            var blockY = Math.floor(tileY / TILES_PER_BLOCK_SIDE);
            tileData[blockX+","+blockY] = data;
        }else{
            
        }
    })
    
    setCenter(centerX+centerOffsetX*0.1, centerY+centerOffsetY*0.1);
    centerOffsetX = centerOffsetY = 0;
}

function undig() {
	var tileX = Math.floor(centerX);
	var tileY = Math.floor(centerY);
	if (!checkTile(tileX, tileY))
		return;
	$.getJSON("/cmapi/dig", {x:tileX, y:tileY, undig:1}, function(json) {
        var err = json.error;
        if (err==0) {
            var data = unbase64(json.data);
            var blockX = Math.floor(tileX / TILES_PER_BLOCK_SIDE);
            var blockY = Math.floor(tileY / TILES_PER_BLOCK_SIDE);
            tileData[blockX+","+blockY] = data;
        }
    })
}

function checkTile(tileX, tileY) {
	var blockX = Math.floor(tileX/TILES_PER_BLOCK_SIDE);
	var blockY = Math.floor(tileY/TILES_PER_BLOCK_SIDE);
	var data = tileData[blockX+","+blockY];
	if (typeof(data) == "undefined" || data == 0) {
		return false;
	} else {
		var x = Math.floor(tileX%TILES_PER_BLOCK_SIDE);
		var y = Math.floor(tileY%TILES_PER_BLOCK_SIDE);
		var idx = y*TILES_PER_BLOCK_SIDE+x;
		var bt = data[Math.floor(idx/8)];
		if ((bt>>(7-Math.floor(idx)%8)) & 1 == 1) {
			return true;
		}
	}
	return false;
}

function frame() {
	var t = new Date().getTime();
	var dt = t - time;

	update(dt);
	draw(dt);

	time = t;
	frame_handle = requestAnimationFrame(frame);
}

function update(dt) {
	if (!checkTile(Math.floor(centerX), Math.floor(centerY))) {
		//return;
	}
	var centerFree = checkTile(Math.floor(centerX), Math.floor(centerY));
	tt = dt;
	var dx = 0;
	var dy = 0;
	var speed = 0.015*dt;
	if (moveLeft && !moveRight)
		dx = -1;
	else if (moveRight && !moveLeft)
		dx = 1;
	if (moveUp && !moveDown)
		dy = -1;
	else if (moveDown && !moveUp)
		dy = 1;

	if (dx == 0 && dy == 0)
		return;
	else if (dx != 0 && dy != 0)
		speed *= 0.7071;

	if (zoom > ZOOM_LOW)
		speed /= Math.pow(1.5, (zoom-ZOOM_LOW));

	if (dx || dy){
		centerOffsetX = centerOffsetY = 0;
	}

	var fx = centerX+speed*dx;
	var ix = Math.floor(fx);
	if (!checkTile(ix, Math.floor(centerY)) && ix != Math.floor(centerX)) {
		if (dx < 0) {
			fx = Math.floor(centerX)+0.001;
			if (centerFree)
				centerOffsetX = -1;
		} else if (dx > 0) {
			fx = Math.floor(centerX)+0.999;
			if (centerFree)
				centerOffsetX = 1;
		}
		ix = Math.floor(fx);
	}
	var fy = centerY+speed*dy;
	var iy = Math.floor(fy);
	if (!checkTile(ix, iy) && iy != Math.floor(centerY)) {
		if (dy < 0) {
			fy = Math.floor(centerY)+0.001;
			if (centerFree)
				centerOffsetY = -1;
		} else if (dy > 0) {
			fy = Math.floor(centerY)+0.999;
			if (centerFree)
				centerOffsetY = 1;
		}
	}
	if (centerOffsetX != 0){
		centerOffsetY = 0;
	}
	setCenter(fx, fy);
}

function draw(dt) {
	canvasAdj();

	if (showMap) {
		ctx.clearRect(0, 0, canvasW, canvasH);
	} else {
		ctx.globalAlpha = 1.0;
		ctx.fillStyle="#dddddd";
		ctx.fillRect(0, 0, canvasW, canvasH);
	}

	if (zoom < ZOOM_LOW) {
		ctx.globalAlpha = 0.8;
		ctx.fillStyle="#000000";
		ctx.fillRect(0, 0, canvasW, canvasH);
		return;
	}
	
	if (zoom >= ZOOM_LOW && zoom <= 18) {
		var tileLen = getTileLen();
		var blockLen = tileLen * TILES_PER_BLOCK_SIDE;
		var left = centerX - canvasW*0.5/tileLen;
		var top = centerY - canvasH*0.5/tileLen;

		var canvasX = Math.floor(-(left%TILES_PER_BLOCK_SIDE)*tileLen);
		var canvasY = Math.floor(-(top%TILES_PER_BLOCK_SIDE)*tileLen);
		var blockX = Math.floor(left/TILES_PER_BLOCK_SIDE);
		var blockY = Math.floor(top/TILES_PER_BLOCK_SIDE);
		binfo = [canvasX, canvasY, blockX, blockY];

		ctx.globalAlpha = 0.8;
		ctx.fillStyle="#000000";

		var x, y, blkX, blkY;
		for (y = canvasY, blkY=blockY; ; y += blockLen, ++blkY) {
			if (y > canvasH)
				break;
			if (y + blockLen < 0)
				continue;
			for (x = canvasX, blkX=blockX; ; x += blockLen, ++blkX) {
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

		ctx.globalAlpha = 1.0;
		ctx.strokeStyle="#ff0000";
		ctx.strokeRect(x, y, tileLen, tileLen);
		if (centerOffsetX || centerOffsetY) {
			ctx.globalAlpha = 0.5;
			ctx.fillStyle="#ff0000";
			ctx.fillRect(x+centerOffsetX*tileLen, y+centerOffsetY*tileLen, tileLen, tileLen);
		}

		if (zoom > ZOOM_LOW) {
			ctx.globalAlpha = 1.0;
			var r = Math.min(zoom - ZOOM_LOW, 2);
			ctx.fillStyle="#ff0000";
			ctx.fillRect(canvasW/2-r, canvasH/2-r, r*2, r*2);
		}
	}
	ctx.font = "400 32px/2 黑体";
	ctx.globalAlpha = 1.0;
	ctx.fillStyle = "#ff0000";
	ctx.textBaseline = "middle";
	ctx.textAlign = "center";
	ctx.fillText("a", 16, 16);
	ctx.fillText("A", 16, 48);
}

var fetchInterval = 0;
function init() {
	$.getJSON("/cmapi/diguserinfo", function(json) {
        var err = json.error;
        if (err==0) {
        	if (json.x != -1) {
        		var x = json.x;
            	var y = json.y;
            	setCenter(x, y);
        	}
        }
        if (centerX == -1) {
        	setCenter(27437.5, 13388.5);
        	$("#login_text").text("Hello Guest!");
        }
        
		fetchBlocks();
		cancelAnimationFrame(frame_handle);
		frame();
		clearInterval(fetchInterval);
		fetchInterval = setInterval("fetchBlocks()", 5000+Math.random()*500);
    });
}

$(document).ready(function(){
	if (googleExist()) {
		var mapOptions = {
	  		zoom: zoom,
	  		mapTypeId: google.maps.MapTypeId.ROADMAP,
	  		disableDefaultUI: true,
		};
		map = new google.maps.Map(document.getElementById("map_canvas"), mapOptions);
		window.map = map;

		google.maps.event.addListenerOnce(map, 'projection_changed', function(){
		    init();
		});
	} else {
		init();
	}

	//time
	var date = new Date;
	time = date.getTime();
	

	

	//zoom
	Mousetrap.bind(["up"], function() {
		zoom += 1;
		zoom = Math.min(ZOOM_HIGH, zoom);
		if (googleExist()) {
			map.setZoom(zoom);
		}
	    $("#info_div").text("zoom:"+zoom);
		return false;
	}, 'keydown');

	Mousetrap.bind(["down"], function() {
		zoom -= 1;
		if (googleExist()) {
			map.setZoom(zoom);
		}
	    $("#info_div").text("zoom:"+zoom);
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

	Mousetrap.bind(["space"], function() {
		dig();
		return false;
	}, "keydown");
 
	Mousetrap.bind(["backspace"], function() {
		undig();
		return false;
	});
	Mousetrap.bind(["ctrl+m"], function() {
		showMap = !showMap;
		// if (showMap && !googleExist()) {
		// 	$.getScript("https://maps.googleapis.com/maps/api/js?key=AIzaSyDV1vKgc2Mku3MKZruYPosTvaxmRw3jvMc&sensor=false", function(data, textStatus, jqxhr) {
		// 	   	var mapOptions = {
		// 	  		zoom: zoom,
		// 	  		mapTypeId: google.maps.MapTypeId.ROADMAP,
		// 	  		disableDefaultUI: true,
		// 		};
		// 		map = new google.maps.Map(document.getElementById("map_canvas"), mapOptions);
		// 		window.map = map;

		// 		google.maps.event.addListenerOnce(map, 'projection_changed', function(){
		// 		    init();
		// 		});
		// 		console.log('ok');
		// 	});
		// }
		return false;
	});

	//mouse
	var draging = false;
	var prevX = 0;
	var prevY = 0;
	$("#dig_canvas").mousedown(function(event) {
		draging = true;
		prevX = event.pageX;
		prevY = event.pageY;
		centerOffsetX = centerOffsetY = 0;
		return false;
	});
	$("#dig_canvas").mousemove(function(event) {
		if (draging) {
			var tileLen = getTileLen();
			var dx = event.pageX - prevX;
			var dy = event.pageY - prevY;
			if (showMap)
				setCenter(centerX-dx/tileLen, centerY-dy/tileLen);
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
		zoom += delta;
		zoom = Math.min(ZOOM_HIGH, zoom);
		if (googleExist()) {
			map.setZoom(zoom);
		}
	    $("#info_div").text("zoom:"+zoom);
	    return false;
	});

	$(window).resize(function() {
		canvasAdj();
		return false;
	});

	setInterval(function(){
		var img = new Image();
		img.src = "https://www.google.com/images/nav_logo117.png";
	}, 30000)

	var username = getCookie("username");
	if (username.length > 0)
		$("#login_text").text("Hello "+username+"!");

	$("#btn_logout").click(function(){
		$("#rps_modal").modal( {
            keyboard: false,
            backdrop: "static"}
        );
        $("#reg_alert").hide();
	});

	$("#btn_sign").click(function(){
		var username=$("#input_username").attr("value");
	    var password=$("#input_password").attr("value");
	    var alert = $("#reg_alert");
	    $.getJSON("/cmapi/reglog", {username:username, password:password}, function(json){
	        var err = json.error;
	        var username = getCookie("username");
	        if (err==0) {
	            bsalert(alert, "success", "Sign in succeed!");
	            $("#login_text").text("Hello "+username+"!");
	            $("#rps_modal").modal('hide');
	        	init();
	        } else {
	            bsalert(alert, "error", "Sign up/in failed:"+err);
	        }
	    })
	    .error(function(){
	        bsalert(alert, "error", "net error");
	    });
	});

	Mousetrap.bind(["enter"], function() {
		
	});
});
