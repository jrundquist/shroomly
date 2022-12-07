const AWS = require("aws-sdk");
const localAuthProxyFn = async (event, context, callback) => {
  callback(null, {
    principalId: "localAuth",
    policyDocument: {
      Version: "2012-10-17",
      Statement: [
        {
          Action: "execute-api:invoke",
          Effect: "Allow",
          Resource: "*",
        },
      ],
    },
  });
};

module.exports = { localAuthProxyFn };
