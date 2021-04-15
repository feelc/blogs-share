#!/bin/bash

g++ -o  client client.cpp -DFAST_OPEN
g++ -o  client server.cpp -DFAST_OPEN