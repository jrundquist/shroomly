import "./App.css";

import * as React from "react";

import useConfig from "./components/useConfig";
import logo from "./logo.svg";
import { Amplify, Auth } from "aws-amplify";
import awsExports from "./aws-exports";
import { Authenticator } from "@aws-amplify/ui-react";

import "@aws-amplify/ui-react/styles.css";

function HitTest() {
  const [res, setRes] = React.useState<String | null>(null);

  React.useEffect(() => {
    Auth.currentSession().then((session) => {
      console.log({ session });
      fetch("/dev/test", {
        method: "POST",
        cache: "no-cache",
        credentials: "same-origin",
        body: JSON.stringify({ hello: "world" }),
        headers: { Authorization: session.idToken.jwtToken },
      }).then((req) => {
        req.json().then((js) => setRes(JSON.stringify(js)));
      });
    });
  }, []);

  if (res === null) {
    return <p>Loading...</p>;
  }

  return <p>Loaded: {res}</p>;
}

/**
 * Our Web Application
 */
export default function App() {
  const config = useConfig();

  console.log({ config });
  Amplify.configure(config.awsConfig);

  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <h1 className="App-title">Welcome to {config.app.TITLE}</h1>
      </header>
      <div className="App-intro">
        <Authenticator>
          {({ signOut, user }) => (
            <main>
              <h1>Hello {user!.username}</h1>
              <button onClick={signOut}>Sign out</button>
              <HitTest />
            </main>
          )}
        </Authenticator>
      </div>
    </div>
  );
}
