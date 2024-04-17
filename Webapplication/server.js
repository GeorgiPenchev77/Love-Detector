import express from "express";
import path from 'path';

const app = express();
const port = 3000;

import { fileURLToPath } from 'url';
import { dirname } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

app.use(express.static(path.join(__dirname, 'public')));


// Define your route handlers
const router = express.Router();

app.use('/', router);

app.listen(port, () => {
    console.log(`Example app listening on port ${port}`);
});
