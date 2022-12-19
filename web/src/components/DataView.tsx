import React from 'react';
import useConfig from './useConfig';

interface Props {
  deviceId: string;
}

interface Datum {
  
}

export const DataView: React.FunctionComponent<Props>({deviceId} => {
     const config = useConfig();
    const [setupComplete, setSetupComplete] = React.useState<boolean>(false);
    const [data, setData] = React.useState<Datum[]>([]);

    React.useEffect(() => {
      Auth.currentCredentials().then((creds) => {
        console.log({ creds });
        api.setConfiguration(
          config.awsConfig.aws_project_region,
          creds.accessKeyId,
          creds.secretAccessKey,
          creds.sessionToken,
        );
        setSetupComplete(true);
      });
    }, [config]);

    React.useEffect(() => {
      if (!setupComplete) return;
      api.listDatabases().then((dbs) => {
        console.log("databases", dbs);
      });

      const DatabaseName = `ShroomlyTimestreams`;
      const TableName = `ShroomlySensorReadings`;

      api
        .query(DatabaseName, TableName, {
          interval: "12h",
          groupInterval: "10m",
          measures: [
            { name: "co2", type: "double" },
            { name: "temperature", type: "double" },
            { name: "humidity", type: "double" },
          ],
          filters: [{ dimension: "device_id", operator: "=", value: deviceId }],
          groupAggregation: "AVG",
        })
        .then((res) => {
          console.log(res);
          setData(
            res.result.Rows.map(({ Data }) => {
              return {
                time: new Date(Data[0].ScalarValue),
                co2: Number(Data[1].ScalarValue) || 0,
                temp: Number(Data[2].ScalarValue),
                hum: Number(Data[3].ScalarValue),
              };
            }).sort((a, b) => a.time - b.time),
          );
        });
    }, [setupComplete, deviceId, setData]);

}