// ==UserScript==
// @name        PhoneLink
// @namespace   net.grepper.phonelink
// @description Turns telephone numbers into tel: links
// @version     1
// @grant       none
// ==/UserScript==

// This function scans through all likely elements and checks for phone numbers
// It returns an array of the elements. NOTE: An element returned does not necessarily
//  have a phone number in it. Some other numbers can be matched as well, such as tracking numbers.
function findPhones() {
    var p = /\d{3}[^\d]+\d{3}[^\d]*\d{4}/;
    var a = document.body.querySelectorAll('div, span, font, a, b, i, td, p');
    var i = a.length;
    var r = [];
    for (var l = 0; l < i; l++) {
        if (a[l].childElementCount === 0 && p.test(a[l].innerHTML) && (!p.href || p.href.length === 0 || document.phonelink.isgoogle)) {
	        r.push(a[l]);
		}
    }
    return r;
}
function swapToLink(el) {
    var p = /\b\+?1?[^\d]{0,2}\d{3}[^\d]+\d{3}[^\d]*\d{4}[^a-zA-Z\d]*\b/;
    //                                  ^     vs   ^, what's the meaning/use?
	var o = /[\+\(\)\-]/;
	var or = /[^\d]/g;
    if (p.test(el.textContent)) {
        var n = el.textContent.replace(or, '');
		if(n.length > 12 || !o.test(el.textContent)) { // || (el.href && el.href.length !== 0)) {
			// Not a phone number
		}
		else {
			if(!el.href || el.href.length === 0 || (document.phonelink.isgoogle && el.title === "Call via Hangouts")) {
        		el.innerHTML = '<a href="tel:' + n + '">' + el.textContent + '</a>';
        	}
        }
			//console.log("Swapped!");
    }
}

function google_recheck() {
	var d = document.querySelector("div._eF");
	if(d)
		swapToLink(d);
	d = document.querySelector("span._Xbe > span > span");
	if(d)
		swapToLink(d);
}

document.phonelink = {};
document.phonelink.firstrun = true;
document.phonelink.recheck = false;
document.phonelink.isgoogle = false;

function mark_recheck() {
	document.phonelink.recheck = true;
}

function do_recheck() {
	if(document.phonelink.recheck) {
		document.phonelink.recheck = false;
		gm_main();
	}
}

function gm_main() {
    var plist = findPhones();
	//console.log("Got list of " + plist.length + " potential phone numbers.");
    var len = plist.length;
    for (var l = 0; l < len; l++) {
        swapToLink(plist[l]);
    }
	if(document.phonelink.firstrun) {
		document.phonelink.firstrun = false;
		//console.log("First run!");
		if(/\.google\.com$/.test(window.location.host)) {
			//console.log("This is Google!");
			document.phonelink.isgoogle = true;
			document.getElementById("main").addEventListener("DOMSubtreeModified", mark_recheck, false);
			window.setInterval(do_recheck, 1500);
		}
	}
}

//console.log("Starting...");
window.addEventListener("load", gm_main, false);
