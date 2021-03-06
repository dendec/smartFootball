module.exports = {
    sendError: sendError,
    sendResponse: function (res, onSuccess, onError) {
        if (typeof onSuccess == "undefined")
            onSuccess = function (doc) {
                res.status(200).json(doc);
            };
        if (typeof onError == "undefined")
            onError = function (err) {
                sendError(res, 500, err);
            };
        return function (err, doc) {
            if (err)
                onError(err);
            else
                onSuccess(doc);
        };
    },
    getCollection: function (collectionName, response) {
        return function (req, res) {
            var db = req.db;
            var collection = db.get(collectionName);
            response(collection, res, req);
        };
    }
};

function sendError(res, statusCode, message) {
    res.status(statusCode).json({"status": "fail", "message": message});
}
