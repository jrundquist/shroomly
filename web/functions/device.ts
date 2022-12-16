import "source-map-support/register";
import { Context, APIGatewayEvent, APIGatewayProxyResultV2 } from "aws-lambda";
import { randomBytes } from "crypto";
import { Iot, IotData } from "aws-sdk";

export async function updateShadow(event: APIGatewayEvent, context: Context): Promise<APIGatewayProxyResultV2> {
  const rideId = randomBytes(16).toString("base64").replace(/\+/g, "-").replace(/\//g, "_").replace(/=/g, "");
  console.log("Received event (", rideId, "): ", event);

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

  const iot = new Iot();
  const endpointResult = new Promise<string>((resolve, reject) => {
    iot.describeEndpoint({}, (err, data) => {
      if (!err) {
        resolve(data.endpointAddress!);
      } else {
        reject(err);
      }
    });
  });
  const iotEndpoint = await endpointResult;
  const iotData = new IotData({ endpoint: iotEndpoint });

  // Because we're using a Cognito User Pools authorizer, all of the claims
  // included in the authentication token are provided in the request context.
  // This includes the username as well as other attributes.
  const username = (event.requestContext.authorizer.claims || {})["cognito:username"] || "UNKNOWN";

  // The body field of the event in a proxy integration is a raw string.
  // In order to extract meaningful values, we need to first parse this string
  // into an object. A more robust implementation might inspect the Content-Type
  // header first and use a different parsing strategy based on that value.
  const requestBody = JSON.parse(event.body ?? "");

  if (requestBody.device_id === undefined || requestBody.state === undefined) {
    return {
      statusCode: 500,
      body: JSON.stringify({
        username,
        request: requestBody,
        error: "Missing device_id or state",
      }),
      headers: {
        "Access-Control-Allow-Origin": "*",
      },
    };
  }

  const params = {
    topic: "$aws/things/${requestBody.device_id}/shadow/update",
    payload: JSON.stringify({ state: { requested: requestBody.state } }),
  };

  const publishResult = new Promise<unknown>((resolve, reject) => {
    iotData.publish(params, (err, res) => {
      if (err) {
        reject(err);
      } else {
        resolve(res);
      }
    });
  });

  try {
    await publishResult;
    return {
      statusCode: 200,
      body: JSON.stringify({
        username,
        response: "Accepted",
      }),
      headers: {
        "Access-Control-Allow-Origin": "*",
      },
    };
  } catch (e) {
    return {
      statusCode: 500,
      body: JSON.stringify({
        username,
        response: "Error",
        error: (e as Error).message,
      }),
      headers: {
        "Access-Control-Allow-Origin": "*",
      },
    };
  }
}
