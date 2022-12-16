import * as React from "react";
import { Amplify, PubSub, Auth } from "aws-amplify";
import useConfig from "./useConfig";
import { AWSIoTProvider } from "@aws-amplify/pubsub/lib/Providers/AWSIotProvider";
import { ZenObservable } from "zen-observable-ts";

interface DeviceProps {
  deviceId: string;
}

declare interface DeviceState {
  fanOn: boolean;
  humidifierOn: boolean;
  growLightOn: boolean;
}

declare interface State {
  desired?: Partial<DeviceState>;
  reported?: Partial<DeviceState>;
}

declare interface UpdateMsg {
  state: State;
}

export const Device: React.FunctionComponent<DeviceProps> = ({ deviceId }) => {
  const config = useConfig();

  const [lastHeard, setLastHeard] = React.useState<unknown | null>(null);
  const [isSetup, setSetup] = React.useState<boolean>(false);
  const [desiredState, setDesiredState] = React.useState<DeviceState | null>(null);
  const [reportedState, setReportedState] = React.useState<DeviceState | null>(null);

  const sensorTopic = React.useMemo(() => `shroomly/${deviceId}/sensors`, [deviceId]);
  const getTopic = React.useMemo(() => `$aws/things/${deviceId}/shadow/get`, [deviceId]);
  const shadowUpdateTopic = React.useMemo(() => `$aws/things/${deviceId}/shadow/update`, [deviceId]);
  const shadowUpdateDocumentsTopic = React.useMemo(() => `$aws/things/${deviceId}/shadow/update/documents`, [deviceId]);
  const getAcceptedTopic = React.useMemo(() => `$aws/things/${deviceId}/shadow/get/accepted`, [deviceId]);

  React.useEffect(() => {
    const subs: ZenObservable.Subscription[] = [];
    Auth.currentSession().then((info) => {
      PubSub.addPluggable(
        new AWSIoTProvider({
          aws_pubsub_region: config.awsConfig.aws_project_region,
          aws_pubsub_endpoint: `wss://${config.awsConfig.aws_mqtt_app}.iot.${config.awsConfig.aws_project_region}.amazonaws.com/mqtt`,
        }),
      );
      console.log("subscribing to: ", sensorTopic);

      subs.push(
        PubSub.subscribe(sensorTopic).subscribe({
          next: ({ value, provider }) => {
            console.log("Message received", value);
            setLastHeard(value);
          },
          error: (error) => console.error("Error subscribing", error),
          complete: () => console.log("Done"),
        }),
      );

      // Sub to Get
      console.log("subscribing to: ", getAcceptedTopic);
      subs.push(
        PubSub.subscribe(getAcceptedTopic).subscribe({
          next: ({ value }) => {
            console.log("\n\nGET:", value);

            setDesiredState(value.state.desired);
            setReportedState(value.state.reported);
          },
          error: (error) => console.error("Error subscribing", error),
          complete: () => console.log("Done"),
        }),
      );

      subs.push(
        PubSub.subscribe(shadowUpdateDocumentsTopic).subscribe({
          next: ({ value }) => {
            console.log("\n\nGET:", value);

            setDesiredState(value.current.state.desired);
            setReportedState(value.current.state.reported);
          },
          error: (error) => console.error("Error subscribing", error),
          complete: () => console.log("Done"),
        }),
      );

      setTimeout(() => {
        PubSub.publish(getTopic, { get: "client" });
      }, 1000);

      setSetup(true);
    });
    return () => {
      for (const s of subs) {
        s.unsubscribe();
      }
    };
  }, [setSetup, setLastHeard, setReportedState, setDesiredState]);

  const forcePublish = React.useCallback(() => {
    PubSub.publish(getTopic, { get: "forced" });
  }, [getTopic]);

  const toggleLight = React.useCallback(() => {
    if (!desiredState) return;
    const newState: Partial<DeviceState> = { growLightOn: !desiredState.growLightOn };
    const update: UpdateMsg = { state: { desired: newState } };
    PubSub.publish(shadowUpdateTopic, update);
  }, [desiredState?.growLightOn, shadowUpdateTopic]);

  const toggleFan = React.useCallback(() => {
    if (!desiredState) return;
    const newState: Partial<DeviceState> = { fanOn: !desiredState.fanOn };
    const update: UpdateMsg = { state: { desired: newState } };
    PubSub.publish(shadowUpdateTopic, update);
  }, [desiredState?.fanOn, shadowUpdateTopic]);

  const toggleHumidifier = React.useCallback(() => {
    if (!desiredState) return;
    const newState: Partial<DeviceState> = { humidifierOn: !desiredState.humidifierOn };
    const update: UpdateMsg = { state: { desired: newState } };
    PubSub.publish(shadowUpdateTopic, update);
  }, [desiredState?.humidifierOn, shadowUpdateTopic]);

  return (
    <>
      <section>
        <h1>Device: {deviceId}</h1>
        <p>Latest Sensor Readings:</p>
        <pre>{JSON.stringify(lastHeard, null, 2)}</pre>

        {desiredState ? (
          <>
            <button onClick={toggleLight}>Turn Light {desiredState.growLightOn ? "Off" : "On"}</button>
            <button onClick={toggleFan}>Turn Fan {desiredState.fanOn ? "Off" : "On"}</button>
            <button onClick={toggleHumidifier}>Turn Humidifier {desiredState.humidifierOn ? "Off" : "On"}</button>
          </>
        ) : (
          <button onClick={forcePublish}>Load State</button>
        )}
      </section>
    </>
  );
};
