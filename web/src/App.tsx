import "./App.css";

import * as React from "react";

import useConfig from "./components/useConfig";
import { Auth } from "aws-amplify";
import { Authenticator } from "@aws-amplify/ui-react";

import "@aws-amplify/ui-react/styles.css";

function HitTest() {
  const config = useConfig();
  const [res, setRes] = React.useState<String | null>(null);

  React.useEffect(() => {
    Auth.currentSession().then((session) => {
      console.log({ session });
      fetch(`${config.app.URL}/test`, {
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
 * Main Application
 */
export default function App() {
  const config = useConfig();

  return (
    <Authenticator>
      {({ signOut, user }) => (
        <main>
          <h1>Hello {user!.username}</h1>
          <button onClick={signOut}>Sign out</button>
          <HitTest />
        </main>
      )}
    </Authenticator>
  );
}
