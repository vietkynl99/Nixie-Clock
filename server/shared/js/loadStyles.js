"use strict";

function loadCSS(url) {
    var link = document.createElement('link');
    link.rel = 'stylesheet';
    link.type = 'text/css';
    link.href = url;

    document.head.appendChild(link);
}

loadCSS(onlinePrefixUrl + '/css/style.min.css');
