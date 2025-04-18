# bixit-server

`bixit-server` is a lightweight service that exposes the full power of the [Bixit](https://github.com/github/bixit) library over HTTP and gRPC.  
It provides fast and configurable (de)serialization of binary messages using format descriptions, enabling seamless integration with external systems, APIs, and test tools.

## 🚀 What It Does

Bixit-server allows clients to:

- 🔍 **Deserialize** raw binary messages into structured JSON
- 🛠 **Serialize** JSON objects back into compact binary formats
- 📦 Use **preconfigured format schemas** or provide custom ones on the fly
- 🌐 Access the service over **HTTP (REST)** or **gRPC**

No need to write binary parsers or recompile format-specific code. Just describe your message once, and let the server handle the rest.

## 🧠 Use Cases

- **Hardware-in-the-loop testing**
- **Simulators and emulators**
- **Remote decoding services**
- **IoT backends with binary payloads**
- **Bridges and gateways between binary protocols and REST APIs**

## 🧩 Architecture Overview

`bixit-server` is built on top of the C++ core library [Bixit](https://github.com/your-org/bixit), and offers two interfaces:

- **HTTP API** – Simple REST-style endpoints
- **gRPC API** – For high-performance, strongly typed interaction

All message formats are managed through **schemas** defined in Bixit’s domain-specific configuration files. These schemas can be:

- Loaded from a **catalog** (directory structure)
- Sent as part of the **request payload** (for ad-hoc use)

## 📥 Example HTTP API

```http
TBD
