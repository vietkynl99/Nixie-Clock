const fs = require('fs');
const axios = require('axios');

const apiKeyFilePath = '../data/weather/apiKey.txt';
const locationKeyFilePath = '../data/weather/locationKey.txt';
var apiKey
var locationKey

try {
    apiKey = fs.readFileSync(apiKeyFilePath, 'utf8').trim();
} catch (err) {
    console.error('Error occurs:', err);
    process.exit(1);
}

try {
    locationKey = fs.readFileSync(locationKeyFilePath, 'utf8').trim();
} catch (err) {
    console.error('Error occurs:', err);
    process.exit(1);
}

if(apiKey.length == 0 || locationKey.length == 0) {
    console.error('No Key found');
    process.exit(1);
}

const weatherApiUrl = 'http://dataservice.accuweather.com/forecasts/v1/daily/1day/';
const url = `${weatherApiUrl}${locationKey}?apikey=${apiKey}&metric=true`

axios.get(url)
    .then(response => {
        console.log(response.data);
    })
    .catch(error => {
        console.log(error);
    });