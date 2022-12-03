import "source-map-support/register";
import { Context, APIGatewayEvent, APIGatewayProxyResultV2 } from "aws-lambda";
import { randomBytes } from "crypto";

export async function testAuth(event: APIGatewayEvent, context: Context): Promise<APIGatewayProxyResultV2> {
  if (!event.requestContext.authorizer) {
    return {
      statusCode: 500,
      body: JSON.stringify({
        Error: "Not authorized",
        Reference: context.awsRequestId,
      }),
      headers: {
        "Access-Control-Allow-Origin": "*",
      },
    };
  }

  const rideId = randomBytes(16).toString("base64").replace(/\+/g, "-").replace(/\//g, "_").replace(/=/g, "");
  console.log("Received event (", rideId, "): ", event);

  // Because we're using a Cognito User Pools authorizer, all of the claims
  // included in the authentication token are provided in the request context.
  // This includes the username as well as other attributes.
  const username = event.requestContext.authorizer.claims["cognito:username"];

  // The body field of the event in a proxy integration is a raw string.
  // In order to extract meaningful values, we need to first parse this string
  // into an object. A more robust implementation might inspect the Content-Type
  // header first and use a different parsing strategy based on that value.
  const requestBody = JSON.parse(event.body ?? "");

  // You can use the callback function to provide a return value from your Node.js
  // Lambda functions. The first parameter is used for failed invocations. The
  // second parameter specifies the result data of the invocation.

  // Because this Lambda function is called by an API Gateway proxy integration
  // the result object must use the following structure.
  return {
    statusCode: 200,
    body: JSON.stringify({
      username,
    }),
    headers: {
      "Access-Control-Allow-Origin": "*",
    },
  };
}
