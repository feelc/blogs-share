#!/bin/bash

g++ -o client client.cpp -DFAST_OPEN
g++ -o server server.cpp -DFAST_OPEN