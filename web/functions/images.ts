import "source-map-support/register";
import {
  Context,
  APIGatewayProxyResultV2,
  APIGatewayEvent,
  IoTEvent,
  Callback,
  APIGatewayProxyStructuredResultV2,
} from "aws-lambda";
import { randomBytes } from "crypto";
import { Iot, IotData, S3 } from "aws-sdk";
import { HTTPHeader } from "aws-sdk/clients/waf";

const BUCKET = "shroomly-timelapse-images";

declare interface UploadRequest {
  base64Image: string;
  S3Folder: string;
  Filename: string;
}
declare interface UploadResponse extends APIGatewayProxyStructuredResultV2 {}

const s3 = new S3();

export async function uploadPost(event: UploadRequest, context: Context, callback: Callback<UploadResponse | null>) {
  const encodedImage = event.base64Image;
  const folder = event.S3Folder;
  const filename = event.Filename;

  let decodedImage = Buffer.from(encodedImage, "base64");
  var filePath = "images/" + folder + "/" + filename + ".jpg";
  var params = {
    Body: decodedImage,
    Bucket: BUCKET,
    Key: filePath,
  };
  s3.upload(params, (err: Error | null, data: S3.ManagedUpload.SendData) => {
    if (err) {
      callback(err, null);
    } else {
      const response = {
        statusCode: 200,
        headers: {
          my_header: "my_value",
        },
        body: JSON.stringify(data),
        isBase64Encoded: false,
      };
      callback(null, response);
    }
  });
}

declare interface Request {
  device_id: string;
  timestamp: number;
  req_id: unknown;
}
export async function initiateUpload(event: IoTEvent<Request>, context: Context): Promise<APIGatewayProxyResultV2> {
  const randId = randomBytes(16).toString("base64").replace(/\+/g, "-").replace(/\//g, "_").replace(/=/g, "");
  console.log("Received event (", randId, "): ", event);
  console.log("Received event:", JSON.stringify(event, null, 2));

  const { device_id, timestamp, req_id } = event;

  const s3 = new S3({ apiVersion: "2006-03-01" });

  const key = `${device_id}/${timestamp}.jpg`;
  const contentType = "image/jpeg";

  const url = s3.getSignedUrl("putObject", {
    Bucket: BUCKET,
    Key: key,
    Expires: 60,
    ContentType: contentType,
  });

  const res = { url, req_id, key, bucket: BUCKET };

  console.log("Response: ", res);

  // TODO implement
  const response = {
    statusCode: 200,
    body: "OK",
  };

  const iot = new Iot();
  const endpointResult = new Promise<string>((resolve, reject) => {
    iot.describeEndpoint({}, (err, data) => {
      if (!err) {
        console.log("Endpoint: ", data.endpointAddress);
        resolve(data.endpointAddress);
      } else {
        reject(err);
      }
    });
  });
  const iotEndpoint = await endpointResult;
  const iotData = new IotData({
    endpoint: iotEndpoint,
    sslEnabled: false,
    httpOptions: { timeout: 5000 },
  });

  const params: IotData.PublishRequest = {
    topic: `shroomly/${device_id}/images/accepted`,
    payload: JSON.stringify(res),
    qos: 0,
  };

  console.log({ params });
  const publishResult = new Promise((resolve, reject) => {
    iotData.publish(params, (err, res) => {
      if (err) {
        console.error("OH NO!", err);
        reject(err);
      } else {
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
  } catch (e: unknown) {
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
