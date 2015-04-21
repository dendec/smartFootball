var express = require('express');
var rh = require('../utils/responseHelper');
var router = express.Router();

/* GET users listing. */
router.get('/', function (req, res, next) {
    var db = req.db;
    var users = db.get('users');
    users.find({}, {}, rh.sendResponse(res));
});

router.get('/rfid/:rfid', function (req, res, next) {
    var db = req.db;
    var users = db.get('users');
    users.findOne({'rfid': req.params.rfid}, {}, rh.sendResponse(res));
});

router.post('/', function (req, res) {
    var db = req.db;
    if ('rfid' in req.body) {
        var users = db.get('users');
        users.findOne({'rfid': req.body.rfid}, {}, rh.sendResponse(res, function (doc) {
            if (doc == null)
                users.insert(req.body, rh.sendResponse(res));
            else
                users.update({'_id': doc._id}, req.body, rh.sendResponse(res));
        }));
    }
    else
        rh.sendError(res, 400, "wrong JSON");
});

router.delete('/', function (req, res) {
    var db = req.db;
    var request = {};
    if (Object.keys(request).length > 0) {
        var users = db.get('users');
        users.remove(request, rh.sendResponse(res));
    }
    else
        rh.sendError(res, 400, "wrong JSON");
});

router.delete('/all', function (req, res) {
    var db = req.db;
    var users = db.get('users');
    users.remove({}, rh.sendResponse(res));
});

module.exports = router;