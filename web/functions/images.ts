import "source-map-support/register";
import { Context, APIGatewayProxyResultV2, IoTEvent } from "aws-lambda";
import { randomBytes } from "crypto";
import { Iot, IotData, S3 } from "aws-sdk";

export async function initiateUpload(event: IoTEvent<{ device_id: string, timestamp: number, req_id: unknown }>, context: Context): Promise<APIGatewayProxyResultV2> {
  const randId = randomBytes(16).toString("base64").replace(/\+/g, "-").replace(/\//g, "_").replace(/=/g, "");
  console.log("Received event (", randId, "): ", event);

  console.log('Received event:', JSON.stringify(event, null, 2));

  const { device_id, timestamp, req_id } = event;

  const s3 = new S3({ apiVersion: '2006-03-01' });

  const key = `${device_id}/${timestamp}.jpg`;
  const contentType = "image/jpeg";
  const bucket = "shroomly-timelapse-images";
  const url = s3.getSignedUrl("putObject", {
    Bucket: bucket,
    Key: key,
    Expires: 60,
    ContentType: contentType,
  });


  const res = { url, req_id, key, bucket };

  // TODO implement
  const response = {
    statusCode: 200,
    body: 'OK',
  };


  const iot = new Iot();
  const endpointResult = new Promise<string>((resolve, reject) => {
    iot.describeEndpoint({}, (err, data) => {
      if (!err) {
        console.log("Endpoint: ", data.endpointAddress)
        resolve(data.endpointAddress);
      }
      else {
        reject(err);
      }
    });
  });
  const iotEndpoint = await endpointResult;
  const iotData = new IotData({ 'endpoint': iotEndpoint });

  const params = {
    topic: `shroomly/${device_id}/images/accepted`,
    payload: JSON.stringify(res)
  };

  console.log({ params });
  const publishResult = new Promise((resolve, reject) => {
    iotData.publish(params, (err, res) => {
      if (err) {
        console.error("OH NO!", err);
        reject(err);
      }
      else {
        resolve(res);
      }
    });
  });

  try {
    await publishResult;
    console.log("Published");
    return {
      statusCode: 200,
      body: JSON.stringify({
        response: "Done",
      }),
      headers: {
        "Access-Control-Allow-Origin": "*",
      },
    };
  }
  catch (e: unknown) {
    return {
      statusCode: 500,
      body: JSON.stringify({
        response: "Error",
        error: (e as Error).message,
      }),
      headers: {
        "Access-Control-Allow-Origin": "*",
      },
    };
  }
}
