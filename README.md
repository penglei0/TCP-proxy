# Introduction

A TCP proxy means to break the connection between a client and the server. The connection is cut into three parts, the first part is between the tcp client and the proxy client, the second part is between the proxy client and the proxy server, ,the third part is between the proxy server and the tcp server. By doing this, we can change the transmission protocol between the proxy client and the proxy server to any protocol we want.

# Architecture

<div align="center" style="text-align:center"> 
<img src="./docs/imgs/tcp_proxy.svg" alt="architecture"></div>
<div align="center">Fig 1.1 TCP proxy architecture</div>

- **Proxy Frontend**: The frontend module is used to redirect the tcp traffic to a local tcp server.
- **Proxy Backend**: The backend module is used to transmit the TCP payload data to the remote by using UDP/KCP or other protocols.
- **Proxy Server**: The proxy server is used to receive the data from the client and transmit the data to the internet according to its original information of the TCP connections.
- **Proxy client**: The proxy client is used to transmit/receive the data to/from the proxy server, and the transmission protocol can be any protocol we want.

# Usages

## Build

## Deploy