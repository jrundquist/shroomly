/**
 * Configuration
 */
import manifest from "../../public/manifest.json";

/** Whether we're running on a local desktop or on AWS Lambda */
const isLocal = process.env.IS_LOCAL || process.env.IS_OFFLINE;

/**
 * Configuration Options
 *
 * IMPORTANT:
 * The config is injected into the client (browser) and accessible through the {@link useConfig}
 * hook. However, due to this behavior, it is important NOT to expose any sensitive information
 * such as passwords or tokens through the config.
 */
const config = {
  /** Application Config */
  app: {
    /** Name of the app is loaded from the `manifest.json` */
    TITLE: manifest.short_name,
    /** Theme is also loaded from the `manifest.json` */
    THEME_COLOR: manifest.theme_color,
    /** URL to our public API Gateway endpoint */
    URL: isLocal ? `http://localhost:3000` : String(process.env.APIGATEWAY_URL),
    /** Where the bundled distribution files (`main.js`, `main.css`) are hosted */
    DIST_URL: isLocal ? "http://localhost:8080" : String(process.env.APP_DIST_URL),
    /** Where the contents of the `public` folder are hosted (might be the same as `config.app.DIST_URL`) */
    PUBLIC_URL: isLocal ? "http://localhost:8080" : String(process.env.APP_PUBLIC_URL),
  },

  awsConfig: {
    all: JSON.stringify(process.env),
    aws_project_region: String(process.env.SERVERLESS_REGION),
    //aws_cognito_identity_pool_id: "us-west-1:117565975709:userpool/us-west-1_eS4RMawSr",
    aws_cognito_region: String(process.env.SERVERLESS_REGION),
    aws_user_pools_id: isLocal ? "us-west-1_eS4RMawSr" : String(process.env.USER_POOL_ID),
    aws_user_pools_web_client_id: isLocal ? "70b372i3a99maehru7ufrq559d" : String(process.env.USER_POOL_CLIENT_ID),
    oauth: {},
    aws_cognito_username_attributes: [],
    aws_cognito_social_providers: [],
    aws_cognito_signup_attributes: ["EMAIL"],
    aws_cognito_mfa_configuration: "OFF",
    aws_cognito_mfa_types: [],
    aws_cognito_password_protection_settings: {
      passwordPolicyMinLength: 8,
      passwordPolicyCharacters: [],
    },
    aws_cognito_verification_mechanisms: ["EMAIL"],
  },
};

export type Config = typeof config;
export default config;
