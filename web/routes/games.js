var express = require('express');
var moment = require('moment');
var rh = require('../utils/responseHelper');
var router = express.Router();

router.get('/', function (req, res, next) {
    var db = req.db;
    var games = db.get('games');
    games.find({}, {}, rh.sendResponse(res));
});

router.get('/:id', function (req, res) {
    var db = req.db;
    var games = db.get('games');
    games.findById(req.params.id, {}, rh.sendResponse(res));
});

router.post('/:id/:team', function (req, res) {
    var db = req.db;
    var games = db.get('games');
    games.findById(req.params.id, {}, rh.sendResponse(res, function (doc) {
        console.log(doc);
        if (doc == null)
            rh.sendError(res, 404, "invalid game id");
        else
            switch (req.params.team) {
                case 'red':
                    doc.red.goals.push(moment() - moment(doc.start));
                    games.update({'_id': doc._id}, doc, rh.sendResponse(res));
                    break;
                case 'blue':
                    doc.blue.goals.push(moment() - moment(doc.start));
                    games.update({'_id': doc._id}, doc, rh.sendResponse(res));
                    break;
                default:
                    rh.sendError(res, 404, "invalid team id");
            }
    }));
});

router.post('/', function (req, res) {
    var db = req.db;
    if (isCorrectGameRequest(req.body)) {
        var games = db.get('games');
        games.insert(getGame(req.body), rh.sendResponse(res));
    }
    else
        rh.sendError(res, 400, "wrong JSON");
});

router.delete('/', function (req, res) {
    var db = req.db;
    var request = {};
    if (Object.keys(request).length > 0) {
        var games = db.get('games');
        games.remove(request, rh.sendResponse(res));
    }
    else
        rh.sendError(res, 400, "wrong JSON");
});

router.delete('/all', function (req, res) {
    var db = req.db;
    var games = db.get('games');
    games.remove({}, rh.sendResponse(res));
});

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
        start: moment().format("YYYY-MM-DD HH:mm:ss.SSS")
    };
    return game;
}

module.exports = router;