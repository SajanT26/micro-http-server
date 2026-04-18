Micro HTTP/1.1 Server

## Overview
This project is a lightweight HTTP/1.1 server built in C using the Winsock2 API. It manually handles TCP connections, parses HTTP requests, and serves static files.

## Features
- TCP socket server
- Manual HTTP request parsing
- Static file serving
- Supports GET requests
- Handles 200, 404, and 405 responses
- Multi-threaded client handling

## How to Run
1. Open MSYS2 MINGW64 terminal  
2. Navigate to project folder  
3. Compile:
   gcc src/server.c -o server -lws2_32  
4. Run:
   ./server  
5. Open browser:
   http://localhost:8080  

## Demo Pages
- / → Home page  
- /about.html → About page  
- /random.html → 404 error  

## Technologies Used
- C  
- Winsock2  
- HTTP/1.1  
- TCP/IP  
