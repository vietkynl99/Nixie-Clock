"use strict";

function loadContent(elementId, url) {
    return new Promise((resolve, reject) => {
        fetch(url)
            .then(response => response.text())
            .then(text => {
                text = text.replaceAll('ONLINE_PREFIX_URL', onlinePrefixUrl)
                document.getElementById(elementId).innerHTML = text;
                resolve();
            })
            .catch(error => {
                console.error('Error:', error);
                reject(error);
            });
    });
}

loadContent('contentContainer', onlinePrefixUrl + '/pages/home/body.html')
    .then(() => {
        console.log('Content loaded successfully. Loading scripts...');
        loadScript(onlinePrefixUrl + '/plugins/chart.min.js')
            .then(function () {
                return loadScript(onlinePrefixUrl + '/plugins/feather.min.js');
            })
            .then(function () {
                return loadScript(onlinePrefixUrl + '/js/script.js');
            })
            .then(function () {
                console.log('Scripts loaded successfully.');
            })
            .catch(function (error) {
                console.error('Error loading script:', error);
            });
    })
    .catch(error => {
        console.error('Error loading content:', error);
    });
