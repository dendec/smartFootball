var express = require('express');
var rh = require('../utils/responseHelper');
var router = express.Router();

/* GET users listing. */
router.get('/', getCollection(function (collection, res) {
    collection.find({}, {}, rh.sendResponse(res));
}));

router.get('/rfid/:rfid', getCollection(function (collection, res, req) {
    collection.findById(req.params.rfid, {}, rh.sendResponse(res));
}));

router.post('/', getCollection(function (collection, res, req) {
    if (isCorrectUserRequest(req.body)) {
        collection.findById(req.body.rfid, {}, rh.sendResponse(res, function (doc) {
            if (doc == null)
                collection.insert(getUser(req.body), rh.sendResponse(res));
            else
                collection.update({'_id': doc._id}, getUser(req.body), rh.sendResponse(res));
        }));
    }
    else
        rh.sendError(res, 400, "wrong JSON");
}));

router.delete('/:rfid', getCollection(function (collection, res, req) {
    var rfid = req.params.rfid;
    console.log(rfid);
    collection.remove({'rfid': rfid}, rh.sendResponse(res));
}));

router.delete('/all', getCollection(function (collection, res) {
    collection.remove({}, rh.sendResponse(res));
}));

function isCorrectUserRequest(userRequest) {
    return ('rfid' in userRequest);
}

function getUser(userRequest) {
    var user = userRequest;
    user._id = user.rfid.toString();
    return user;
}

function getCollection(response) {
    return rh.getCollection("users", response);
}

module.exports = router;