const OS = require('os');
const ASSERT = require('assert');
const FS = require('fs');
const CHILD_PROCESS = require('child_process');
const HTTPS = require('https');
const HTTP = require("http");
const URL = require("url");
const PATH = require("path");
const CRYPTO = require("crypto");
const ICONV = require("iconv-lite");
const XML = require("node-xml-lite");

/*
* Http Server wrapper
*
*/
function HttpServer(port){
	this._Port = port;	
if (typeof HttpServer._initialized === 'undefined'){
////////////////////////////////////////////////////////////////////////////
	HttpServer.prototype.start = function(onRequest){
		var server = HTTP.createServer(onRequest);		
		server.listen(this._Port, "127.0.0.1");
		console.info("Server is listening on: " + this._Port);
	};
////////////////////////////////////////////////////////////////////////////	
	HttpServer._initialized = true;
}
}

/*
* Class: Http Client Wrapper
* var client = new HttpClient();
* client.httpRequest({
*				"url" : "http://cn.bing.com/search?q=sdff&go=%E6%8F%90%E4%BA%A4&qs=bs&form=QBLH", 
*				"callback" : function(c, u, h, b){console.log(b.toString());}
*				});
*/
function HttpClient(){
// Member functions by: 
	// None	
// * Daynamic prototype
if ((typeof HttpClient._initialized === 'undefined')){
//////////////////////////////////////////////////////////////////////////////////////////////////
	HttpClient.prototype.parseUrl = function(url){
		if ( ('undefined' === typeof url) || null == url || "" == url ){
			console.error("invliad paramters: " + url);
			return null;
		}
		var options = URL.parse(url);
		if (options == null || (typeof options.protocol === 'undefined')){
			console.error("url parse failed");
			return null;
		}
		return options;
	};
	
	HttpClient.prototype.getClient = function(options){
		var client = null;
		if (options.protocol == "https:"){
			client = HTTPS;
		}else if (options.protocol == "http:"){
			client = HTTP;
		}else{
			console.error("protocol not supported");
		}
		return client;
	};
	
// Http Get request
	HttpClient.prototype.httpRequest = function (req){
		var options = this.parseUrl(req.url);
		if (options == null){
			if ('undefined' !== typeof req.callback){
				req.callback(501, null, null, null);
			}
			return ;
		}
		
		var client = this.getClient(options);
		if (client == null){
			if ('undefined' !== typeof req.callback){
				req.callback(501, null, null, null);
			}
			return ;
		}
		
		if ((typeof req.body) !== 'undefined'){
			options.method = "POST";
		}
		
		var request = client.request(options, (incMsg) => {
			console.log(`Got http response, status code: ${incMsg.statusCode}`);
			
			var statusCode = incMsg.statusCode;
			
			var body = new Buffer(0);

			incMsg.on("data", function onData(chunk){
				body = Buffer.concat([ body, chunk], body.length + chunk.length);
			});
			
			incMsg.on("end", function onEnd(){
				console.info("Http response done");
				if ('undefined' !== typeof req.callback){
					req.callback(statusCode, req.url, incMsg.headers, body);
				}
				return ;
			});
			
			incMsg.on("error", function onError(){
				console.error("Failed to get http response");
				if ('undefined' !== typeof req.callback){
					req.callback(501, null, null, null);
				}
				return ;
			});
		});
		
		if ('undefined' !== typeof req.body){
			request.setHeader("content-type", "text/plain; charset=utf-8");
			request.setHeader("accept", "*/*");
			request.write(JSON.stringify(req.body));
		}
		
		request.on("error", function onError(){
			console.error("Failed to http request via GET");
			if ('undefined' !== typeof req.callback){
				req.callback(501, null, null, null);
			}
			return ;
		});
		
		request.end();	
	};
//////////////////////////////////////////////////////////////////////////////////////////////////
// * Endof Dynamic prototype
	HttpClient._initialized = true;
}
}

process.title = "BitMail Backend Application Server";
process.chdir(PATH.dirname(module.filename));
process.on('SIGINT', ()=>{
		console.log("Signal <interrupt>");
		process.exit(0);
	});
process.on('SIGBREAK', ()=>{
		console.log("Signal <Ctrl-Break>");
		process.exit(0);
});
process.on('SIGKIL', ()=>{
		console.log("Signal <User Kill>");
		process.exit(0);
	});		
process.on('SIGHUP', ()=>{
		console.log("Signal <Console colosed>");
		process.exit(0);
});	
process.on('SIGPIPE', ()=>{
		console.log("Signal <Socket IO>");
});	
console.log(`CWD : ${process.cwd()}`);
console.log(`PID : ${process.pid}`);
console.log(`TIME: ${new Date()}`);

var backend = new HttpServer(10086);
backend.start(onRequest);

// Handlers dispatch
function onRequest(req, resp) {
	console.log("Request: " + (req.url));
	
	var body = new Buffer(0);
	req.on('data', function (chunk){
		try{
			body = Buffer.concat([body, chunk], body.length + chunk.length);
		}catch(e){
			console.error("failed to read: " + e);
		}
		
	});
	req.on('end', function(){
		try{
			return httpHandler(req, body, resp);
		}catch(e){
			console.error("failed to process: " + e);
		}
	});
};

function httpHandler(req, body, resp)
{
	console.log("Request Url: " + req.url);
	console.log("Request Body:" + JSON.stringify(body));
	
	var request = URL.parse(req.url);
	
	console.log(JSON.stringify(request));
	
	if (typeof request.pathname === 'undefined'
		|| request.pathname == null
		|| request.pathname.toLowerCase() != "/search"){
		resp.writeHead(404, {'Content-Type': 'text/plain'});
		resp.write("no this handler");
		resp.end();
		return ;
	}
	
	if (typeof request.query === 'undefined'
		|| request.query == null
		|| request.query == ""){
		try{			
			request = JSON.parse(body);
		}catch(e){
			request = null;
		}
	}	

	if (request != null 
		&& typeof request.query !== 'undefined'
		&& request.query != null
		&& request.query != "")
	{
		// http://stackoverflow.com/questions/11530159/doing-http-requests-through-a-socks5-proxy-in-nodejs
		// https://github.com/mattcg/socks5-https-client
		// the default socks5 daemon running at: 127.0.0.1:1080
		var shttps = require('socks5-https-client');
		shttps.get({
			socksHost: "127.0.0.1",
			socksPort: 1080,
			hostname: 'encrypted.google.com',
			path: '/search?site=&source=hp&q=' + request.query,
			rejectUnauthorized: true // This is the default.
		}, function(res) {
			var page = "";
			res.setEncoding('utf8');
			res.on('readable', function() {
				//console.log("Receiving ...");
				page += res.read(); // Log response to console.
			});
			res.on('end', function(){
				console.log("Response Done");
				resp.writeHead(200, {'Content-Type': 'text/html; charset=UTF-8'});							
				resp.write(page);	
				resp.end();
			});
			res.on('error', function(){
				resp.writeHead(501, {'Content-Type': 'text/html; charset=UTF-8'});	
				resp.write("Failed to receive response");
				resp.end();
			});
		}).on("error", function(){
				resp.writeHead(501, {'Content-Type': 'text/html; charset=UTF-8'});
				resp.write("Failed to request upstream");
				resp.end();
		});
		
		/**
		var client = new HttpClient();
		client.httpRequest({
			"url"      : "http://cn.bing.com/search?q=" + request.query,
			"callback" : function (code, url, headers, body){
							var retobj = {};
							
							if (code == 200){
								retobj.errcode = 0;
								retobj.result = bingParse(body);
							}else{
								retobj.errcode = 501;
								retobj.result = {};
							}
							resp.writeHead(200, {'Content-Type': 'application/json'});							
							resp.write(JSON.stringify(retobj));
							resp.end();
						}
		});	
		*/
	}else{
		console.error("bad request");
		resp.writeHead(400, {'Content-Type': 'text/plain'});							
		resp.write("Bad Request");
		resp.end();
	}
}


