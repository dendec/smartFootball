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

router.delete('/', getCollection(function (collection, res, req) {
    var request = req.body;
    if (Object.keys(request).length > 0)
        collection.remove(request, rh.sendResponse(res));
    else
        rh.sendError(res, 400, "wrong JSON");
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