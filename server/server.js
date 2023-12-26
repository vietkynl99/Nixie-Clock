const express = require('express');
const cors = require('cors');
const path = require('path');

const app = express();
const port = 3000;

const folderPath = path.join(__dirname, 'shared');

app.use(cors());

app.use(express.static(folderPath));

app.listen(port, () => {
    console.log(`Server is running at http://localhost:${port}`);
});
