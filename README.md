# Introduction

A TCP proxy means to break the connection between a client and the server. The connection is cut into three parts, the first part is between the tcp client and the proxy client, the second part is between the proxy client and the proxy server, ,the third part is between the proxy server and the tcp server. By doing this, we can change the transmission protocol between the proxy client and the proxy server to any protocol we want.

# Architecture
