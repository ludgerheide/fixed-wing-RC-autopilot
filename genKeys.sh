#!/bin/sh
openssl genrsa -out controller.priv.pem 2048
openssl rsa -pubout -in controller.priv.pem -out controller.pub.pem

openssl genrsa -out drone.priv.pem 2048
openssl rsa -pubout -in drone.priv.pem -out drone.pub.pem

mv drone.pub.pem Drone\ Viewer\ \(iOS\)/drone.pub.pem
mv controller.priv.pem Drone\ Viewer\ \(iOS\)/controller.priv.pem

mv controller.pub.pem DroneComms/controller.pub.pem
mv drone.priv.pem DroneComms/drone.priv.pem