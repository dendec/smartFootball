var express = require('express');
var moment = require('moment');
var rh = require('../utils/responseHelper');
var router = express.Router();

router.get('/', getCollection(function (collection, res) {
    collection.find({}, {}, rh.sendResponse(res));
}));

router.get('/:id', getCollection(function (collection, res, req) {
    collection.findById(req.params.id, {}, rh.sendResponse(res));
}));

router.post('/:id/:team', getCollection(function (collection, res, req) {
    collection.findById(req.params.id, {}, rh.sendResponse(res, function (doc) {
        if (doc == null)
            rh.sendError(res, 404, "invalid game id");
        else {
            var currentMoment = new Date().getTime();
            switch (req.params.team) {
                case 'red':
                    doc.red.goals.push(currentMoment - doc.start);
                    break;
                case 'blue':
                    doc.blue.goals.push(currentMoment - doc.start);
                    break;
                default:
                    rh.sendError(res, 404, "invalid team id");
                    return;
            }
            if ((doc.red.goals.length == 10) || (doc.blue.goals.length == 10))
                doc.stop = currentMoment;
            collection.update({'_id': doc._id}, doc, rh.sendResponse(res));
        }
    }));
}));

router.post('/', getCollection(function (collection, res, req) {
    console.log(req.body);
    if (isCorrectGameRequest(req.body)) {
        var game = getGame(req.body);
        collection.insert(game, rh.sendResponse(res));
    }
    else
        rh.sendError(res, 400, "wrong JSON");
}));

router.delete('/', getCollection(function (collection, res, req) {
    var request = req.body;
    if (Object.keys(request).length > 0)
        collection.remove(request, rh.sendResponse(res));
    else
        rh.sendError(res, 400, "empty JSON");
}));

router.delete('/all', getCollection(function (collection, res) {
    collection.remove({}, rh.sendResponse(res));
}));

//ra - red attacker
//rd - red defender
//ba - blue attacker
//bd - blue defender
function isCorrectGameRequest(gameRequest) {
    return (('id' in gameRequest) && ('ra' in gameRequest) && ('rd' in gameRequest) && ('ba' in gameRequest) && ('bd' in gameRequest));
}

function getGame(gameRequest) {
    var game = {
        _id: gameRequest.id.toString(),
        red: {
            attacker: gameRequest.ra,
            defender: gameRequest.rd,
            goals: []
        },
        blue: {
            attacker: gameRequest.ba,
            defender: gameRequest.bd,
            goals: []
        },
        start: new Date().getTime()
    };
    return game;
}

function getCollection(response) {
    return rh.getCollection("games", response);
}

module.exports = router;
